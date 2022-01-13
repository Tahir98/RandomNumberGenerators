#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "Utility/Properties.h"
#include "Utility/Properties.h"

class ImGuiLayer {
private:
    static GLFWwindow* window;

public:
    static void init(GLFWwindow* window);

    static void beginFrame();

    static void renderFrame();

    static void terminate();
};





