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
#include <cstddef> // for offsetof
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp" 
#include "chunkMesher.hpp"

// settings
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true; // First mouse movement flag
float yaw = -90.0f; // Yaw is initialized to -90 degrees to look along the negative Z-axis
float pitch = 0.0f; // Pitch is initialized to 0 degrees
float fov = 45.0f; // Field of view for perspective projection
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;

// timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

/**
 * @brief Callback function to adjust the viewport when the window is resized
 * 
 * @param window The GLFW window that was resized
 * @param width New width of the window
 * @param height New height of the window
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Callback function for mouse movement
 * 
 * @param window 
 * @param xpos 
 * @param ypos 
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Callback function for mouse scroll events
 * 
 * @param window The GLFW window
 * @param xoffset The scroll offset in the x-direction (not used)
 * @param yoffset The scroll offset in the y-direction
 */
void scroll_callback(GLFWwindow* window, double /* xoffset */, double yoffset);

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
    ChunkMesher::MeshData mesh;
    glm::mat4 model = glm::mat4(1.0f);
    // Camera setup
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection;

    /** TODO: TEST */
    Chunk chunk{};
    setBlock(chunk, 0, 0, 0, BlockType::Stone);
    mesh = ChunkMesher::generateMesh(chunk);
    /** TEST END */

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
    glfwSetCursorPosCallback(window, mouse_callback);
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    // Face culling
    glEnable(GL_CULL_FACE);
    // Set the viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture the mouse cursor
    glfwSetScrollCallback(window, scroll_callback);

    // Vertex data and buffer setup
    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkMesher::Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);

    // position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesher::Vertex), (void*)offsetof(ChunkMesher::Vertex, x));
    glEnableVertexAttribArray(0);
    // color attributes
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ChunkMesher::Vertex), (void*)offsetof(ChunkMesher::Vertex, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO
    glBindVertexArray(0); // Unbind the VAO (bind when rendering)


    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Input
        processInput(window);

        // Rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        shader.setMat4("model", model);

        shader.setMat4("view", view);
        shader.setMat4("proj", projection);




        glBindVertexArray(VAO);
        // Set polygon mode to wireframe for debugging
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set polygon mode to wireframe
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
} // main

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

void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, windowWidth, windowHeight);
}

void mouse_callback(GLFWwindow* /* window */, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) // initially set to true
    {
        lastX = 0;
        lastY = 0;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.001f; // Change this
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* /* window */, double /* xoffset */, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}

void processInput(GLFWwindow *window)
{
    // Close the window if the ESC key is pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}