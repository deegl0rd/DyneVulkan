#pragma once

#include "Window/WindowHandler.hpp"
#include "VulkanBackend/DyneRenderer.hpp"
#include "VulkanBackend/DyneTexture.hpp"
#include "VulkanBackend/DyneSwapchain.hpp"
#include "VulkanBackend/DyneModel.hpp"
#include "VulkanBackend/DyneDescriptors.hpp"
#include "Engine/GameObject.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <vector>

namespace Dyne 
{
    class Application
    {
    public:
        //Window settings
        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;
        const std::string WNAME = "Editor";

        Application();
        ~Application();

        Application(const Application&) = delete;
        void operator=(const Application&) = delete;

        void run();

    private:
        void allocateBuffers();
        void loadGameObjects();
        void cleanup();

        //Initialize window and the vulkan device
        WindowHandler app{ WIDTH, HEIGHT, WNAME };
        DyneDevice appDevice{ app };
        DyneRenderer appRenderer{ app, appDevice };

        VkSampler textureSampler;
        std::vector<std::unique_ptr<DyneBuffer>> uboBuffers;
        std::unique_ptr<DyneDescriptorPool> globalPool{};
        GameObject::Map gameObjects;
    };
}

