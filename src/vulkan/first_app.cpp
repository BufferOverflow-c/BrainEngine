//
// Created by - on 6/12/24.
//

#include "../../include/vulkan/first_app.h"
#include "vulkan/vulkan_core.h"

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//~ std
#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace brn {

struct SimplePushConstantData {
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

FirstApp::FirstApp() {
  loadModels();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(brnDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!brnWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(brnDevice.device());
}

void FirstApp::sierpinski(std::vector<BrnModel::Vertex> &vertices, int depth,
                          glm::vec2 left, glm::vec2 right, glm::vec2 top) {
  if (depth <= 0) {
    vertices.push_back({top});
    vertices.push_back({right});
    vertices.push_back({left});
  } else {
    glm::vec<2, float> leftTop = 0.5f * (left + top);
    glm::vec<2, float> rightTop = 0.5f * (right + top);
    glm::vec<2, float> leftRight = 0.5f * (left + right);
    sierpinski(vertices, depth - 1, left, leftRight, leftTop);
    sierpinski(vertices, depth - 1, leftRight, right, rightTop);
    sierpinski(vertices, depth - 1, leftTop, rightTop, top);
  }
}

void FirstApp::loadModels() {
  std::vector<BrnModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  // sierpinski(vertices, 5, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.f, -0.5f});
  brnModel = std::make_unique<BrnModel>(brnDevice, vertices);
}

void FirstApp::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(brnDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout");
  }
}

void FirstApp::createPipeline() {
  assert(brnSwapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  BrnPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = brnSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  brnPipeline = std::make_unique<BrnPipeline>(
      brnDevice,
      "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/"
      "simple_shader.vert.spv",
      "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/"
      "simple_shader.frag.spv",
      pipelineConfig);
}

void FirstApp::recreateSwapChain() {
  auto extent = brnWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = brnWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(brnDevice.device());
  brnSwapChain = nullptr;
  if (brnSwapChain == nullptr) {
    brnSwapChain = std::make_unique<BrnSwapChain>(brnDevice, extent);
  } else {
    brnSwapChain = std::make_unique<BrnSwapChain>(brnDevice, extent,
                                                  std::move(brnSwapChain));
    if (brnSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }
  // if render pass compatible do nothing else
  createPipeline();
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(brnSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = brnDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(brnDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate command buffers");
  }
}

void FirstApp::freeCommandBuffers() {
  vkFreeCommandBuffers(brnDevice.device(), brnDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

void FirstApp::recordCommandBuffer(int imageIndex) {
  static int frame = 0;
  frame = (frame + 1) % 1000;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = brnSwapChain->getRenderPass();
  renderPassInfo.framebuffer = brnSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = brnSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.f};
  clearValues[1].depthStencil = {1.f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport;
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(brnSwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(brnSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, brnSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  brnPipeline->bind(commandBuffers[imageIndex]);
  brnModel->bind(commandBuffers[imageIndex]);

  for (int i{}; i < 4; i++) {
    SimplePushConstantData push{};
    push.offset = {-0.5f + frame * 0.02f, -0.4f + i * 0.25f};
    push.color = {0.0f, 0.0f, 0.2f + 0.2f * i};

    vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);

    brnModel->draw(commandBuffers[imageIndex]);
  }

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }
}

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  auto result = brnSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image");
  }

  recordCommandBuffer(imageIndex);

  result = brnSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                              &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      brnWindow.wasWindowResized()) {
    brnWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swap chain image");
  }
}
} // namespace brn
