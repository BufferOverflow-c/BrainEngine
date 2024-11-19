#include "../../include/vulkan/brain_descriptors.h"

// std
#include <cassert>
#include <stdexcept>

namespace brn {

// *************** Descriptor Set Layout Builder *********************

BrnDescriptorSetLayout::Builder &BrnDescriptorSetLayout::Builder::addBinding(
    uint32_t binding, VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags, uint32_t count) {
  assert(bindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<BrnDescriptorSetLayout>
BrnDescriptorSetLayout::Builder::build() const {
  return std::make_unique<BrnDescriptorSetLayout>(brnDevice, bindings);
}

// *************** Descriptor Set Layout *********************

BrnDescriptorSetLayout::BrnDescriptorSetLayout(
    BrnDevice &brnDevice,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : brnDevice{brnDevice}, bindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount =
      static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

  if (vkCreateDescriptorSetLayout(brnDevice.device(), &descriptorSetLayoutInfo,
                                  nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

BrnDescriptorSetLayout::~BrnDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(brnDevice.device(), descriptorSetLayout,
                               nullptr);
}

// *************** Descriptor Pool Builder *********************

BrnDescriptorPool::Builder &
BrnDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType,
                                        uint32_t count) {
  poolSizes.push_back({descriptorType, count});
  return *this;
}

BrnDescriptorPool::Builder &
BrnDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
  poolFlags = flags;
  return *this;
}
BrnDescriptorPool::Builder &
BrnDescriptorPool::Builder::setMaxSets(uint32_t count) {
  maxSets = count;
  return *this;
}

std::unique_ptr<BrnDescriptorPool> BrnDescriptorPool::Builder::build() const {
  return std::make_unique<BrnDescriptorPool>(brnDevice, maxSets, poolFlags,
                                             poolSizes);
}

// *************** Descriptor Pool *********************

BrnDescriptorPool::BrnDescriptorPool(
    BrnDevice &brnDevice, uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes)
    : brnDevice{brnDevice} {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;

  if (vkCreateDescriptorPool(brnDevice.device(), &descriptorPoolInfo, nullptr,
                             &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

BrnDescriptorPool::~BrnDescriptorPool() {
  vkDestroyDescriptorPool(brnDevice.device(), descriptorPool, nullptr);
}

bool BrnDescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorSet &descriptor) const {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this
  // case, and builds a new pool whenever an old pool fills up. But this is
  // beyond our current scope
  if (vkAllocateDescriptorSets(brnDevice.device(), &allocInfo, &descriptor) !=
      VK_SUCCESS) {
    return false;
  }
  return true;
}

void BrnDescriptorPool::freeDescriptors(
    std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(brnDevice.device(), descriptorPool,
                       static_cast<uint32_t>(descriptors.size()),
                       descriptors.data());
}

void BrnDescriptorPool::resetPool() {
  vkResetDescriptorPool(brnDevice.device(), descriptorPool, 0);
}

// *************** Descriptor Writer *********************

BrnDescriptorWriter::BrnDescriptorWriter(BrnDescriptorSetLayout &setLayout,
                                         BrnDescriptorPool &pool)
    : setLayout{setLayout}, pool{pool} {}

BrnDescriptorWriter &
BrnDescriptorWriter::writeBuffer(uint32_t binding,
                                 VkDescriptorBufferInfo *bufferInfo) {
  assert(setLayout.bindings.count(binding) == 1 &&
         "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1 &&
         "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

BrnDescriptorWriter &
BrnDescriptorWriter::writeImage(uint32_t binding,
                                VkDescriptorImageInfo *imageInfo) {
  assert(setLayout.bindings.count(binding) == 1 &&
         "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1 &&
         "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool BrnDescriptorWriter::build(VkDescriptorSet &set) {
  bool success =
      pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void BrnDescriptorWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(pool.brnDevice.device(), writes.size(), writes.data(),
                         0, nullptr);
}

} // namespace brn
