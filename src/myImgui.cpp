/**
 * @file myImgui.cpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief Source file for ImGui interface, implements functions for initializing and rendering the ImGui interface
 * @version 0.1
 * @date 2026-16-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "myImgui.hpp"
#include <iostream>

void imGuiInit(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Dark mode
    ImGui::StyleColorsDark();

    // Link GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void imGuiRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));

    // FRACTAL INFO
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::Begin("Technical Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(1.15f); // Zvětšení textu o 15%

    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Performance: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Separator();

    ImGui::End();

    ImGui::PopStyleVar(2); // Pop the two style variables we pushed

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return;
}