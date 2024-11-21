#ifndef BRAIN_FRAME_INFO_H
#define BRAIN_FRAME_INFO_H

#include "brain_camera.h"
#include "brain_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace brn {
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  BrnCamera &camera;
  VkDescriptorSet globalDescriptorSet;
  BrnGameObject::Map &gameObjects;
};

} // namespace brn

#endif // !BRAIN_FRAME_INFO_H
