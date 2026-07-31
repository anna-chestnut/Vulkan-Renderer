#pragma once

#include "vr_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vr {

class VrDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(VrDevice &vrDevice) : vrDevice{vrDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<VrDescriptorSetLayout> build() const;

   private:
    VrDevice &vrDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  VrDescriptorSetLayout(
      VrDevice &vrDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~VrDescriptorSetLayout();
  VrDescriptorSetLayout(const VrDescriptorSetLayout &) = delete;
  VrDescriptorSetLayout &operator=(const VrDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  VrDevice &vrDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class VrDescriptorWriter;
};

class VrDescriptorPool {
 public:
  class Builder {
   public:
    Builder(VrDevice &vrDevice) : vrDevice{vrDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<VrDescriptorPool> build() const;

   private:
    VrDevice &vrDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  VrDescriptorPool(
      VrDevice &vrDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~VrDescriptorPool();
  VrDescriptorPool(const VrDescriptorPool &) = delete;
  VrDescriptorPool &operator=(const VrDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  VrDevice &vrDevice;
  VkDescriptorPool descriptorPool;

  friend class VrDescriptorWriter;
};

class VrDescriptorWriter {
 public:
  VrDescriptorWriter(VrDescriptorSetLayout &setLayout, VrDescriptorPool &pool);

  VrDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  VrDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  VrDescriptorSetLayout &setLayout;
  VrDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace vr