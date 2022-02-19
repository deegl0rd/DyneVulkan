#pragma once

#include "DyneDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Dyne {

    class DyneDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(DyneDevice& device) : _deviceRef{ device } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<DyneDescriptorSetLayout> build() const;

        private:
            DyneDevice& _deviceRef;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        DyneDescriptorSetLayout(
            DyneDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~DyneDescriptorSetLayout();
        DyneDescriptorSetLayout(const DyneDescriptorSetLayout&) = delete;
        DyneDescriptorSetLayout& operator=(const DyneDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        DyneDevice& _deviceRef;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class DyneDescriptorWriter;
    };

    class DyneDescriptorPool {
    public:
        class Builder {
        public:
            Builder(DyneDevice& device) : _deviceRef{ device } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<DyneDescriptorPool> build() const;

        private:
            DyneDevice& _deviceRef;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        DyneDescriptorPool(
            DyneDevice& device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~DyneDescriptorPool();
        DyneDescriptorPool(const DyneDescriptorPool&) = delete;
        DyneDescriptorPool& operator=(const DyneDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        DyneDevice& _deviceRef;
        VkDescriptorPool descriptorPool;

        friend class DyneDescriptorWriter;
    };

    class DyneDescriptorWriter {
    public:
        DyneDescriptorWriter(DyneDescriptorSetLayout& setLayout, DyneDescriptorPool& pool);

        DyneDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        DyneDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        DyneDescriptorSetLayout& setLayout;
        DyneDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace Dyne