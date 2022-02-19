#pragma once

#include "../VulkanBackend/DyneModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Dyne
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation;

		//Matrix operation order: translation * rY * rX * rZ * scale
		//Rotation convention uses Tait-Bryan angles with order: Y(1), X(2), Z(3)
		//https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();

		glm::vec3 getX();
		glm::vec3 getY();
		glm::vec3 getZ();
	};

	struct PointLightComponent
	{
		float lightIntensity = 1.0f;
	};

	class GameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;

		static GameObject createGameObject()
		{
			static id_t currentID = 0;
			return GameObject(currentID++);
		}

		static GameObject makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		id_t getID() { return id; }

		glm::vec3 color{};
		TransformComponent transform;

		std::shared_ptr<DyneModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		GameObject(id_t objectID) : id{objectID} {}

		id_t id;
	};
}

