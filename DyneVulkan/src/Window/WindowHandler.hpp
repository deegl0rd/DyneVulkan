#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Dyne {

	class WindowHandler
	{
	public:
		WindowHandler(unsigned int width, unsigned int height, std::string name);
		~WindowHandler();

		WindowHandler(const WindowHandler&) = delete;
		void operator=(const WindowHandler&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(handle); };
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; };
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		
		GLFWwindow* getHandle() { return handle; };

	private:
		static void framebufferResizeCallback(GLFWwindow* handle, int width, int height);
		void init();

		unsigned int width;
		unsigned int height;
		bool framebufferResized = false;

		std::string name;
		GLFWwindow* handle;
	};

}