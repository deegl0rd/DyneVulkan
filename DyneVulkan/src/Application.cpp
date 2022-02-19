#include "Application.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Engine/InputHandler.hpp"
#include "Engine/Systems/DefaultRenderSystem.hpp"
#include "Engine/Systems/PointLightSystem.hpp"
#include "Engine/Camera.hpp"

#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>
#include <numeric>


namespace Dyne 
{
	Application::Application()
	{
		globalPool = DyneDescriptorPool::Builder(appDevice)
			.setMaxSets(DyneSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DyneSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DyneSwapchain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		allocateBuffers();
		DyneTexture::createTextureSampler(appDevice, textureSampler);
		auto tex = DyneTexture::createTextureFromFile(appDevice, "textures/concrete.jpg");

		auto globalSetLayout = DyneDescriptorSetLayout::Builder(appDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(DyneSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto uboBufferInfo = uboBuffers[i]->descriptorInfo();
			
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = tex->imageView();
			imageInfo.sampler = textureSampler;

			DyneDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &uboBufferInfo)
				.writeImage(1, &imageInfo)
				.build(globalDescriptorSets[i]);
		}

		DefaultRenderSystem defaultRenderSystem(appDevice, appRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		PointLightRenderSystem pointLightSystem(appDevice, appRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

		Camera camera{};
		GameObject cameraObject = GameObject::createGameObject();
		cameraObject.transform.translation.z = -2.5f;
		InputHandler cameraController{};
		cameraController.setMouseEnabled(&app, true);

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!app.shouldClose())
		{
			glfwPollEvents();

			//Time calculation
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			frameTime = glm::min(frameTime, 100.0f);

			//Camera controller & view & projection matrix setup
			cameraController.moveObjectInPlaneXZ(&app, 1.0f, cameraObject);
			camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);
			camera.setPerspectiveProjection(glm::radians(90.0f), appRenderer.getAspectRatio(), 0.1f, 100.0f);
			
			if (auto commandBuffer = appRenderer.beginFrame())
			{
				int frameIndex = appRenderer.getFrameIndex();
				FrameInfo frameInfo
				{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightSystem.update(frameInfo, ubo);

				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				appRenderer.beginSwapChainRenderPass(commandBuffer);
				defaultRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				appRenderer.endSwapChainRenderPass(commandBuffer);
				appRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(appDevice.device());
		cleanup();
	}

	void Application::cleanup()
	{
		vkDestroySampler(appDevice.device(), textureSampler, nullptr);
	}

	void Application::allocateBuffers()
	{
		//Global Ubo buffer allocation
		uboBuffers = std::vector<std::unique_ptr<DyneBuffer>>(DyneSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<DyneBuffer>
				(
					appDevice,
					sizeof(GlobalUbo),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					);
			uboBuffers[i]->map();
		}

		//Texture buffer allocation
	}

	void Application::loadGameObjects()
	{
		std::shared_ptr<DyneModel> model = DyneModel::createModelFromFile(appDevice, "models/smooth_vase.obj");
		std::shared_ptr<DyneModel> quadModel = DyneModel::createModelFromFile(appDevice, "models/quad.obj");

		auto gameObj = GameObject::createGameObject();
		gameObj.model = model;
		gameObj.transform.translation = { -1.0f, 0.0f, 0.0f };
		gameObj.transform.scale = glm::vec3(5.0f);
		gameObjects.emplace(gameObj.getID(), std::move(gameObj));

		auto gameObj2 = GameObject::createGameObject();
		gameObj2.model = model;
		gameObj2.transform.translation = { 1.0f, 0.0f, 0.0f };
		gameObj2.transform.scale = glm::vec3(5.0f);
		gameObjects.emplace(gameObj2.getID(), std::move(gameObj2));

		auto gameObj3 = GameObject::createGameObject();
		gameObj3.model = quadModel;
		gameObj3.transform.translation = { 0.0f, 0.5f, 0.0f };
		gameObj3.transform.scale = glm::vec3(5.0f);
		gameObjects.emplace(gameObj3.getID(), std::move(gameObj3));

		std::vector<glm::vec3> lightColors
		{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = GameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate
			(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.0f, -1.0f, 0.0f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));

			gameObjects.emplace(pointLight.getID(), std::move(pointLight));
		}
	}
}