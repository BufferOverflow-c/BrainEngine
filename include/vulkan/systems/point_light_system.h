#ifndef POINT_LIGHT_SYSTEM_H
#define POINT_LIGHT_SYSTEM_H

#include "brain_camera.h"
#include "brain_device.h"
#include "brain_frame_info.h"
#include "brain_game_object.h"
#include "brain_pipeline.h"

//~ std
#include <memory>
#include <vector>

namespace brn {

class PointLightSystem {
public:
  PointLightSystem(BrnDevice &device, VkRenderPass renderPass,
                   VkDescriptorSetLayout globalSetLayout);
  ~PointLightSystem();

  //~ Delete copy constructors
  PointLightSystem(const PointLightSystem &) = delete;
  PointLightSystem &operator=(const PointLightSystem &) = delete;

  void update(FrameInfo &frameInfo, GlobalUbo &ubo);
  void render(FrameInfo &frameInfo);

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  BrnDevice &brnDevice;

  std::unique_ptr<BrnPipeline> brnPipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace brn

#endif // POINT_LIGHT_SYSTEM_H
