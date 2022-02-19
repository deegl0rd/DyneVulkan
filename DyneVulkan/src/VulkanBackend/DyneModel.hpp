#pragma once

#include "DyneDevice.hpp"
#include "DyneBuffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Dyne 
{
	class DyneModel
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

			bool operator==(const Vertex& other) const
			{
				return 
					position == other.position && 
					color == other.color && 
					normal == other.normal && 
					uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		DyneModel(DyneDevice& device, const DyneModel::Builder& builder);
		~DyneModel();

		DyneModel(const DyneModel&) = delete;
		DyneModel& operator=(const DyneModel&) = delete;

		static std::unique_ptr<DyneModel> createModelFromFile(DyneDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		DyneDevice& _deviceRef;

		std::unique_ptr<DyneBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<DyneBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
