#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/Camera.h"

class InputHandler {
public:
    static Camera* camera;
    
    // 1. New State Tracking Arrays
    static bool keys[1024];           // Is the key currently held down?
    static bool keysProcessed[1024];  // Has the "press" event already been handled?

    static void Init(GLFWwindow* window, Camera* cam);
    static void ProcessInput(GLFWwindow* window, float deltaTime);
    
    // 2. New Callback Signatures (Must be static for GLFW)
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    // 3. The "Single Tap" Logic Helper
    static bool OnKeyPress(int key);

    // Returns true while Left Shift is held (sprint modifier)
    static bool IsSprinting();
};