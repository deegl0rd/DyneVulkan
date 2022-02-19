#include "InputHandler.hpp"

#include <limits>
#include <iostream>

namespace Dyne
{
	void InputHandler::moveObjectInPlaneXZ(WindowHandler* window, float dt, GameObject& gameObject)
	{
		if (glfwGetMouseButton(window->getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) setMouseEnabled(window, true);

		glm::vec3 rotate{ 0.0f, 0.0f, 0.0f };

		bool isWindowActive = glfwGetWindowAttrib(window->getHandle(), GLFW_FOCUSED);
		if (isMouseEnabled && isWindowActive)
		{
			glfwGetCursorPos(window->getHandle(), &mouseX, &mouseY);
			int centerX = window->getExtent().width / 2;
			int centerY = window->getExtent().height / 2;

			if (firstMouse)
			{
				glfwSetCursorPos(window->getHandle(), centerX, centerY);
				firstMouse = false;
				mouseLastX = mouseX;
				mouseLastY = mouseY;
			}

			if (didMousePosChange())
			{
				glfwSetCursorPos(window->getHandle(), centerX, centerY);
				mouseOffsetX = mouseX - centerX;
				mouseOffsetY = centerY - mouseY;

				mouseLastX = mouseX;
				mouseLastY = mouseY;

				rotate.y += mouseOffsetX;
				rotate.x += mouseOffsetY;
			}

			if (glfwGetKey(window->getHandle(), keys.x) == GLFW_PRESS) setMouseEnabled(window, false);
		}
		else
		{
			setMouseEnabled(window, false);
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float pitch = gameObject.transform.rotation.x;
		float yaw = gameObject.transform.rotation.y;

		const glm::vec3 forwardDir{ sin(yaw), -sin(pitch), cos(yaw)};
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };
		if (glfwGetKey(window->getHandle(), keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window->getHandle(), keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window->getHandle(), keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window->getHandle(), keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window->getHandle(), keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window->getHandle(), keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glfwGetKey(window->getHandle(), keys.lshift) == GLFW_PRESS) speedBoost = true;
		if (glfwGetKey(window->getHandle(), keys.rshift) == GLFW_PRESS) speedBoost = true;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform.translation += (speedBoost ? moveSpeed * speedMultiplier : moveSpeed) * dt * glm::normalize(moveDir);
		}
		speedBoost = false;
	}
}