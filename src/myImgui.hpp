/**
 * @file myImgui.hpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief Header file for ImGui interface, defines functions for initializing and rendering the ImGui interface
 * @version 0.1
 * @date 2026-16-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MY_IMGUI_HPP
#define MY_IMGUI_HPP
#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/**
 * @brief Initializes ImGui
 * 
 * @param window The GLFW window to which ImGui will be attached
 */
void imGuiInit(GLFWwindow *window);

/**
 * @brief Renders the ImGui interface
 * 
 */
void imGuiRender();



#endif // MY_IMGUI_HPP