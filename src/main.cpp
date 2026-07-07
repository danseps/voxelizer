/**
 * @file main.cpp
 * @author Daniel Šeps (daniel.seps@proton.me)
 * @brief 
 * @version 0.1
 * @date 2026-07-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "chunkMesher.hpp" //TODO:

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

/**
 * @brief Callback function to adjust the viewport when the window is resized
 * 
 * @param window The GLFW window that was resized
 * @param width New width of the window
 * @param height New height of the window
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Processes input for the application - listeners
 * 
 * @param window The GLFW window
 */
void processInput(GLFWwindow *window);

/**
 * @brief Initializes GLFW and creates an OpenGL context
 * 
 */
void glfwInitialize();

/**
 * @brief Main entry point of the application
 * 
 * @return int Exit code of the application
 */
int main ()
{

    glfwInitialize();

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "voxelizer", nullptr, nullptr);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    // ------ OpenGL Initialized -------

    std::cout << "Hello, from voxelizer!\n";

    Shader shader("res/shaders/vertexShader.vs", "res/shaders/fragmentShader.fs");
    shader.use();

    // Callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void glfwInitialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    // Set GLFW window hints for OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

inline void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, windowWidth, windowHeight);
} 

void processInput(GLFWwindow *window)
{
    // Close the window if the ESC key is pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}