#include "../../include/vulkan/brain_renderer.h"
#include "vulkan/vulkan_core.h"

//~ std
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace brn {

BrnRenderer::BrnRenderer(BrnWindow &window, BrnDevice &device)
    : brnWindow{window}, brnDevice{device} {
  recreateSwapChain();
  createCommandBuffers();
}

BrnRenderer::~BrnRenderer() { freeCommandBuffers(); }

void BrnRenderer::recreateSwapChain() {
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
    std::shared_ptr<BrnSwapChain> oldSwapChain = std::move(brnSwapChain);
    brnSwapChain =
        std::make_unique<BrnSwapChain>(brnDevice, extent, oldSwapChain);

    if (!oldSwapChain->compareSwapFormats(*brnSwapChain.get())) {
      throw std::runtime_error(
          "Swap chain image(or depth) format has changed!");
    }
  }
  // if render pass compatible do nothing else
  // TODO createPipeline();
}

void BrnRenderer::createCommandBuffers() {
  commandBuffers.resize(brnSwapChain->MAX_FRAMES_IN_FLIGHT);

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

void BrnRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(brnDevice.device(), brnDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer BrnRenderer::beginFrame() {
  assert(!isFrameStarted && "Can't call beginFrame while already in progress");
  auto result = brnSwapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image");
  }
  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }
  return commandBuffer;
}

void BrnRenderer::endFrame() {
  assert(isFrameStarted && "Can't call endFrame while frame is now started");

  auto commandBuffer = getCurrentCommandBuffer();
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }

  auto result =
      brnSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      brnWindow.wasWindowResized()) {
    brnWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swap chain image");
  }

  isFrameStarted = false;
  currentFrameIndex =
      (currentFrameIndex = 1) % BrnSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void BrnRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call beginSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin render pass on command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = brnSwapChain->getRenderPass();
  renderPassInfo.framebuffer = brnSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = brnSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.f};
  clearValues[1].depthStencil = {1.f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
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
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void BrnRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call endSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end render pass on command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);
}
} // namespace brn
