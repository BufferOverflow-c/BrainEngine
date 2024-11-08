#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "brain_device.h"
#include "brain_game_object.h"
#include "brain_pipeline.h"

//~ std
#include <memory>
#include <vector>

namespace brn {

class SimpleRenderSystem {
public:
  SimpleRenderSystem(BrnDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  //~ Delete copy constructors
  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<BrnGameObject> &gameObjects);

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  BrnDevice &brnDevice;

  std::unique_ptr<BrnPipeline> brnPipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace brn

#endif // SIMPLE_RENDER_SYSTEM_H
