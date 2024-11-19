#ifndef BRAIN_DESCRIPTORS_H
#define BRAIN_DESCRIPTORS_H

#include "brain_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace brn {

class BrnDescriptorSetLayout {
public:
  class Builder {
  public:
    Builder(BrnDevice &brnDevice) : brnDevice{brnDevice} {}

    Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType,
                        VkShaderStageFlags stageFlags, uint32_t count = 1);
    std::unique_ptr<BrnDescriptorSetLayout> build() const;

  private:
    BrnDevice &brnDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  BrnDescriptorSetLayout(
      BrnDevice &brnDevice,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~BrnDescriptorSetLayout();
  BrnDescriptorSetLayout(const BrnDescriptorSetLayout &) = delete;
  BrnDescriptorSetLayout &operator=(const BrnDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const {
    return descriptorSetLayout;
  }

private:
  BrnDevice &brnDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class BrnDescriptorWriter;
};

class BrnDescriptorPool {
public:
  class Builder {
  public:
    Builder(BrnDevice &brnDevice) : brnDevice{brnDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<BrnDescriptorPool> build() const;

  private:
    BrnDevice &brnDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  BrnDescriptorPool(BrnDevice &brnDevice, uint32_t maxSets,
                    VkDescriptorPoolCreateFlags poolFlags,
                    const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~BrnDescriptorPool();
  BrnDescriptorPool(const BrnDescriptorPool &) = delete;
  BrnDescriptorPool &operator=(const BrnDescriptorPool &) = delete;

  bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                          VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

private:
  BrnDevice &brnDevice;
  VkDescriptorPool descriptorPool;

  friend class BrnDescriptorWriter;
};

class BrnDescriptorWriter {
public:
  BrnDescriptorWriter(BrnDescriptorSetLayout &setLayout,
                      BrnDescriptorPool &pool);

  BrnDescriptorWriter &writeBuffer(uint32_t binding,
                                   VkDescriptorBufferInfo *bufferInfo);
  BrnDescriptorWriter &writeImage(uint32_t binding,
                                  VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

private:
  BrnDescriptorSetLayout &setLayout;
  BrnDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

} // namespace brn

#endif
