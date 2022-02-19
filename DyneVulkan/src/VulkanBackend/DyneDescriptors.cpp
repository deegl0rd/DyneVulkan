#include "DyneDescriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace Dyne {

    // *************** Descriptor Set Layout Builder *********************

    DyneDescriptorSetLayout::Builder& DyneDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) 
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<DyneDescriptorSetLayout> DyneDescriptorSetLayout::Builder::build() const 
    {
        return std::make_unique<DyneDescriptorSetLayout>(_deviceRef, bindings);
    }

    // *************** Descriptor Set Layout *********************

    DyneDescriptorSetLayout::DyneDescriptorSetLayout(DyneDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : _deviceRef{ device }, bindings{ bindings }
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            _deviceRef.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DyneDescriptorSetLayout::~DyneDescriptorSetLayout() 
    {
        vkDestroyDescriptorSetLayout(_deviceRef.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    DyneDescriptorPool::Builder& DyneDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) 
    {
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    DyneDescriptorPool::Builder& DyneDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) 
    {
        poolFlags = flags;
        return *this;
    }
    DyneDescriptorPool::Builder& DyneDescriptorPool::Builder::setMaxSets(uint32_t count) 
    {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<DyneDescriptorPool> DyneDescriptorPool::Builder::build() const 
    {
        return std::make_unique<DyneDescriptorPool>(_deviceRef, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    DyneDescriptorPool::DyneDescriptorPool(
        DyneDevice& device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : _deviceRef{ device }
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(_deviceRef.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    DyneDescriptorPool::~DyneDescriptorPool()
    {
        vkDestroyDescriptorPool(_deviceRef.device(), descriptorPool, nullptr);
    }

    bool DyneDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(_deviceRef.device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void DyneDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        vkFreeDescriptorSets(
            _deviceRef.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void DyneDescriptorPool::resetPool() 
    {
        vkResetDescriptorPool(_deviceRef.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    DyneDescriptorWriter::DyneDescriptorWriter(DyneDescriptorSetLayout& setLayout, DyneDescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    DyneDescriptorWriter& DyneDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
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

    DyneDescriptorWriter& DyneDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
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

    bool DyneDescriptorWriter::build(VkDescriptorSet& set) 
    {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) 
        {
            return false;
        }
        overwrite(set);
        return true;
    }

    void DyneDescriptorWriter::overwrite(VkDescriptorSet& set) 
    {
        for (auto& write : writes) 
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool._deviceRef.device(), writes.size(), writes.data(), 0, nullptr);
    }

}