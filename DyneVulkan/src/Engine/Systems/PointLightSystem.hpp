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
    class PointLightRenderSystem
    {
    public:

        PointLightRenderSystem(DyneDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightRenderSystem();

        PointLightRenderSystem(const PointLightRenderSystem&) = delete;
        PointLightRenderSystem operator=(const PointLightRenderSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        DyneDevice& _deviceRef;

        std::unique_ptr<DynePipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}

