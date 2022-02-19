#pragma once

#include "../Window/WindowHandler.hpp"
#include "DyneSwapchain.hpp"
#include "DyneModel.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace Dyne
{
    class DyneRenderer
    {
    public:
        DyneRenderer(WindowHandler& window, DyneDevice& device);
        ~DyneRenderer();

        DyneRenderer(const DyneRenderer&) = delete;
        void operator=(const DyneRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); };
        float getAspectRatio() const { return swapChain->extentAspectRatio(); };
        bool isFrameInProgress() const { return isFrameStarted; };

        VkCommandBuffer getCurrentCommandBuffer() const 
        { 
            assert(isFrameStarted && "Cannot get commandbuffer when frame not in progress!");
            return commandBuffers[currentFrameIndex]; 
        };

        int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapchain();

        //Initialize window and the vulkan device
        WindowHandler& _windowRef;
        DyneDevice& _deviceRef;
        std::unique_ptr<DyneSwapchain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;
    };
}

