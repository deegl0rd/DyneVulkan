#include "WindowHandler.hpp"

#include <stdexcept>

namespace Dyne {

	WindowHandler::WindowHandler(unsigned int width, unsigned int height, std::string name) : width(width), height(height), name(name)
	{
		init();
	}

	WindowHandler::~WindowHandler()
	{
		glfwDestroyWindow(handle);
		glfwTerminate();
	}

	void WindowHandler::framebufferResizeCallback(GLFWwindow* handle, int width, int height)
	{
		auto window = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(handle));
		window->framebufferResized = true;
		window->width = width;
		window->height = height;
	}

	void WindowHandler::init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(handle, this);
		glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);
	}

	void WindowHandler::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, handle, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
}
