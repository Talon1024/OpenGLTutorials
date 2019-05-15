#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void processInput(GLFWwindow *window);

int main(int argc, char** argv) {
    // Set GLFW hints so that OpenGL version 3.3 is used
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Make the window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Learn OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD to manage OpenGL extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set up viewport, and resize callback
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);

    // Render loop - do not quit until I quit
    while (!glfwWindowShouldClose(window))
    {
        // Handle input
        processInput(window);

        // Render stuff
        glClearColor(0.0, 0.75, 1.0, 1.0); // Set clear colour in RGBA
        glClear(GL_COLOR_BUFFER_BIT); // Can also be GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT

        // Display rendered frame while waiting for the other frame to render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Release all GLFW resources and exit
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}