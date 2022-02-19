#include "DefaultRenderSystem.hpp"

#include "../GameObject.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace Dyne
{
	struct DefaultPushConstants
	{
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	DefaultRenderSystem::DefaultRenderSystem(DyneDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : _deviceRef(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	DefaultRenderSystem::~DefaultRenderSystem()
	{
		vkDestroyPipelineLayout(_deviceRef.device(), pipelineLayout, nullptr);
	}

	void DefaultRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DefaultPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(_deviceRef.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void DefaultRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		DynePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<DynePipeline>(
			_deviceRef,
			"shaders/shader.vert.spv",
			"shaders/shader.frag.spv",
			pipelineConfig);
	}

	void DefaultRenderSystem::renderGameObjects(FrameInfo& frameInfo)
	{
		pipeline->bind(frameInfo.commandBuffer);
		vkCmdBindDescriptorSets
		(
			frameInfo.commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipelineLayout, 
			0, 1, 
			&frameInfo.globalDescriptorSet, 
			0, 
			nullptr
		);

		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;

			if (obj.model == nullptr) continue;

			DefaultPushConstants push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants
			(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(DefaultPushConstants),
				&push
			);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}