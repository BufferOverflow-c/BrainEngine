
#include "../../include/vulkan/simple_render_system.h"
#include "vulkan/vulkan_core.h"

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//~ std
#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace brn {

struct SimplePushConstantData {
  glm::mat4 transform{1.f};
  alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(BrnDevice &device,
                                       VkRenderPass renderPass)
    : brnDevice{device} {
  createPipelineLayout();
  createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(brnDevice.device(), pipelineLayout, nullptr);
}
void SimpleRenderSystem::createPipelineLayout() {
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

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  BrnPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  brnPipeline = std::make_unique<BrnPipeline>(
      brnDevice,
      "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/"
      "simple_shader.vert.spv",
      "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/"
      "simple_shader.frag.spv",
      pipelineConfig);
}
void SimpleRenderSystem::renderGameObjects(
    VkCommandBuffer commandBuffers, std::vector<BrnGameObject> &gameObjects,
    const BrnCamera &camera) {
  brnPipeline->bind(commandBuffers);

  auto projectionView = camera.getProjection() * camera.getView();

  for (auto &obj : gameObjects) {

    SimplePushConstantData push{};
    push.color = obj.color;
    push.transform = projectionView * obj.transform.mat4();

    vkCmdPushConstants(commandBuffers, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffers);
    obj.model->draw(commandBuffers);
  }
}
} // namespace brn
