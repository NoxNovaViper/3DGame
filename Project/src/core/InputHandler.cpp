#include "core/InputHandler.h"

// 1. Initialize the static members (REQUIRED in C++)
Camera* InputHandler::camera = nullptr;
bool InputHandler::keys[1024] = { false };
bool InputHandler::keysProcessed[1024] = { false };

// Keep track of mouse position
static float lastX = 640, lastY = 360;
static bool firstMouse = true;

void InputHandler::Init(GLFWwindow* window, Camera* cam) {
    camera = cam;
    
    // Connect the GLFW window to our static functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    
    // Hide and lock the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
            keysProcessed[key] = false; // Reset for OnKeyPress
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
            keysProcessed[key] = false;
        }
    }
}

bool InputHandler::OnKeyPress(int key) {
    if (key >= 0 && key < 1024 && keys[key] && !keysProcessed[key]) {
        keysProcessed[key] = true;
        return true;
    }
    return false;
}

void InputHandler::ProcessInput(GLFWwindow* window, float deltaTime) {
    if (!camera) return;

    // Use quotes because your function expects a const char* (string)
    if (keys[GLFW_KEY_W]) camera->ProcessKeyboard("FORWARD", deltaTime);
    if (keys[GLFW_KEY_S]) camera->ProcessKeyboard("BACKWARD", deltaTime);
    if (keys[GLFW_KEY_A]) camera->ProcessKeyboard("LEFT", deltaTime);
    if (keys[GLFW_KEY_D]) camera->ProcessKeyboard( "RIGHT", deltaTime);

    if (OnKeyPress(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}
void InputHandler::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // Reversed: y-coords go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    if (camera) {
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}
bool InputHandler::IsSprinting() {
    return keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT];
}
