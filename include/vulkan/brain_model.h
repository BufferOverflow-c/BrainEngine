//
// Created by - on 6/14/24.
//

#ifndef BRAIN_MODEL_H
#define BRAIN_MODEL_H

#include "brain_device.h"
#include <cstdint>

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//~ std
#include <memory>
#include <vector>

namespace brn {
class BrnModel {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();

    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color &&
             normal == other.normal && uv == other.uv;
    }
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    void loadModels(const std::string &filePath);
  };

  BrnModel(BrnDevice &device, const BrnModel::Builder &builder);
  ~BrnModel();

  // Delete copy constructors
  BrnModel(const BrnModel &) = delete;
  BrnModel &operator=(const BrnModel &) = delete;

  static std::unique_ptr<BrnModel>
  createModelFromFile(BrnDevice &device, const std::string &filePath);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  BrnDevice &brnDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;

  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint32_t> &indices);
};

} // namespace brn

#endif // BRN_MODEL_H
