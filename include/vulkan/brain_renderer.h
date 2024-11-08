
#ifndef BRAIN_RENDERER_H
#define BRAIN_RENDERER_H

#include "brain_device.h"
#include "brain_swap_chain.h"
#include "brain_window.h"

//~ std
#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

namespace brn {

class BrnRenderer {
public:
  BrnRenderer(BrnWindow &window, BrnDevice &device);
  ~BrnRenderer();

  //~ Delete copy constructors
  BrnRenderer(const BrnRenderer &) = delete;
  BrnRenderer &operator=(const BrnRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const {
    return brnSwapChain->getRenderPass();
  }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  BrnWindow &brnWindow;
  BrnDevice &brnDevice;
  std::unique_ptr<BrnSwapChain> brnSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{};
  bool isFrameStarted{false};
};

} // namespace brn

#endif // BRAIN_RENDERER_H
