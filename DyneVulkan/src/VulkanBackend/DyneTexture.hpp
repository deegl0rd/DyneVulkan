#pragma once

#include "DyneDevice.hpp"
#include "DyneBuffer.hpp"

#include <memory>
#include <vector>

namespace Dyne
{
	class DyneTexture
	{
	public:
		static std::unique_ptr<DyneTexture> createTextureFromFile
		(
			DyneDevice& device,
			const std::string& filepath
		);

		struct Builder
		{
			void createTextureImage
			(
				DyneDevice& device,
				const std::string& filepath
			);

			VkImage bTextureImage;
			VkDeviceMemory bTextureImageMemory;
		};

		DyneTexture(DyneDevice& device, const DyneTexture::Builder& builder);
		~DyneTexture();

		DyneTexture(const DyneTexture&) = delete;
		DyneTexture& operator=(const DyneTexture&) = delete;

		VkImage image() { return textureImage; }
		VkImageView imageView() { return textureImageView; }

		static void createTextureSampler(DyneDevice& device, VkSampler& sampler);

	private:
		static void transitionImageLayout(DyneDevice& device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkImageView createImageView(VkImage image, VkFormat format);

		DyneDevice& _deviceRef;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
	};
}

