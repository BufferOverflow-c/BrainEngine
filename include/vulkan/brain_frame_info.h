#ifndef BRAIN_FRAME_INFO_H
#define BRAIN_FRAME_INFO_H

#include "brain_camera.h"
#include "brain_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace brn {

#define MAX_LIGHTS 10

struct PointLight {
  glm::vec4 position{}; // ignore w
  glm::vec4 color{};    // w is intensity
};

struct GlobalUbo {
  glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
  glm::mat4 inverseView{1.f};
  glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.02f};
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
};

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
