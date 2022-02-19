#pragma once

#include "GameObject.hpp"
#include "../Window/WindowHandler.hpp"

namespace Dyne
{
    class InputHandler
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int lshift = GLFW_KEY_LEFT_SHIFT;
            int rshift = GLFW_KEY_RIGHT_SHIFT;
            int x = GLFW_KEY_X;
        };

        void moveObjectInPlaneXZ(WindowHandler* window, float dt, GameObject& gameObject);
        void setMouseEnabled(WindowHandler* window ,bool value)
        { 
            isMouseEnabled = value; 
            glfwSetInputMode(window->getHandle(), GLFW_CURSOR, value ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
        }

    private:
        bool didMousePosChange()
        {
            return (!(mouseX == mouseLastX && mouseY == mouseLastY));
        };

        double mouseX, mouseY;
        double mouseLastX, mouseLastY;
        double mouseOffsetX, mouseOffsetY;

        bool firstMouse = true;
        bool isMouseEnabled = false;

        float moveSpeed{ 0.0012f };
        float lookSpeed{ 0.013f };
        bool speedBoost = false;
        float speedMultiplier{ 2.5f };

        KeyMappings keys{};
	};
}