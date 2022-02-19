#include "DyneRenderer.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace Dyne
{

	DyneRenderer::DyneRenderer(WindowHandler& window, DyneDevice& device) : _windowRef(window), _deviceRef(device)
	{
		recreateSwapchain();
		createCommandBuffers();
	}

	DyneRenderer::~DyneRenderer()
	{
		freeCommandBuffers();
	}

	void DyneRenderer::createCommandBuffers()
	{
		commandBuffers.resize(DyneSwapchain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _deviceRef.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(_deviceRef.device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void DyneRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			_deviceRef.device(),
			_deviceRef.getCommandPool(),
			static_cast<float>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	void DyneRenderer::recreateSwapchain()
	{
		auto extent = _windowRef.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = _windowRef.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_deviceRef.device());

		if (swapChain == nullptr)
		{
			swapChain = std::make_unique<DyneSwapchain>(_deviceRef, extent);
		}
		else
		{
			std::shared_ptr<DyneSwapchain> oldSwapchain = std::move(swapChain);
			swapChain = std::make_unique<DyneSwapchain>(_deviceRef, extent, oldSwapchain);

			if (!oldSwapchain->compareSwapFormats(*swapChain.get()))
			{
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
	}

	VkCommandBuffer DyneRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while already in progress!");

		auto result = swapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void DyneRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress!");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _windowRef.wasWindowResized())
		{
			_windowRef.resetWindowResizedFlag();
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % DyneSwapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void DyneRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

		//CLEAR SCREEN
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f }; //clear color
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void DyneRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}
