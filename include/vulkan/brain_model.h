//
// Created by - on 6/14/24.
//

#ifndef BRAIN_MODEL_H
#define BRAIN_MODEL_H

#include "brain_device.h"

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//~ std
#include <vector>

namespace brn {
class BrnModel {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();
  };

  BrnModel(BrnDevice &device, const std::vector<Vertex> &verticies);
  ~BrnModel();

  // Delete copy constructors
  BrnModel(const BrnModel &) = delete;
  BrnModel &operator=(const BrnModel &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  BrnDevice &brnDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  void createVertexBuffers(const std::vector<Vertex> &vertices);
};

} // namespace brn

#endif // BRN_MODEL_H
