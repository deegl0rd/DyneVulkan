#pragma once

#include "../../VulkanBackend/DynePipeline.hpp"
#include "../../VulkanBackend/DyneFrameInfo.hpp"
#include "../../VulkanBackend/DyneModel.hpp"
#include "../GameObject.hpp"
#include "../Camera.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <vector>

namespace Dyne
{
    class DefaultRenderSystem
    {
    public:

        DefaultRenderSystem(DyneDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~DefaultRenderSystem();

        DefaultRenderSystem(const DefaultRenderSystem&) = delete;
        DefaultRenderSystem operator=(const DefaultRenderSystem&) = delete;

        void renderGameObjects(FrameInfo& frameInfo);


    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        DyneDevice& _deviceRef;

        std::unique_ptr<DynePipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}

