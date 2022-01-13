#include "ImGuiLayer.h"
#include "Log/Log.h"
#include "ImGuiThemes.h"
#include "Utility/Properties.h"

GLFWwindow* ImGuiLayer::window;
Properties Window::properties;

void ImGuiLayer::init(GLFWwindow* window) {
    ImGuiLayer::window = window;

    const char* glsl_version = "#version 430";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // EnableKeyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // EnableGamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // EnableDocking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // EnableMulti-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
     // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

     // When viewports are enabled we tweak WindowRounding/WindowBg soplatform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 10.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.Colors[ImGuiCol_TitleBg] = ImVec4(1,0,0,1);
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    
    ImGuiThemes::DarkTheme1(style);
    float fontSize = 18.0f;// *2.0f;
    io.Fonts->AddFontFromFileTTF("Fonts/arial.ttf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Fonts/arial.ttf", fontSize);

   

    APP_LOG_INFO("ImGui initialized succesfully");
}

void ImGuiLayer::beginFrame() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //APP_LOG_INFO("ImGui frame begin");
}

void ImGuiLayer::renderFrame() {
    

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2(Window::properties.width, Window::properties.height);

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so wesave/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also callglfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    //APP_LOG_INFO("ImGui frame rendered");
}

void ImGuiLayer::terminate() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    APP_LOG_INFO("ImGui terminated");
}
