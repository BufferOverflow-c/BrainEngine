#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "brain_camera.h"
#include "brain_device.h"
#include "brain_frame_info.h"
#include "brain_game_object.h"
#include "brain_pipeline.h"

//~ std
#include <memory>
#include <vector>

namespace brn {

class SimpleRenderSystem {
public:
  SimpleRenderSystem(BrnDevice &device, VkRenderPass renderPass,
                     VkDescriptorSetLayout globalSetLayout);
  ~SimpleRenderSystem();

  //~ Delete copy constructors
  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo);

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  BrnDevice &brnDevice;

  std::unique_ptr<BrnPipeline> brnPipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace brn

#endif // SIMPLE_RENDER_SYSTEM_H
