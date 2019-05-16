#include "glad.h"
#include <GLFW/glfw3.h>
#include "config.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

// Tutorial 2: Hello Triangle
// Part 2: A rectangle. Mainly for comparison with tut2.cpp
// https://learnopengl.com/Getting-started/Hello-Triangle

bool compileShader(const char* filename, unsigned int type, unsigned int &id);
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
        return 1;
    }

    // Set up viewport, and resize callback
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);

    // Define vertex array
    float vertices[] = {
        // first triangle
         0.5,  0.5, 0.0,  // top right
         0.5, -0.5, 0.0,  // bottom right
        -0.5,  0.5, 0.0,  // top left 
        // second triangle
         0.5, -0.5,  0.0,  // bottom right
        -0.5, -0.5,  0.0,  // bottom left
        -0.5,  0.5,  0.0   // top left
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate vertex buffer and upload it to the GPU
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Give OpenGL information about how the vertex buffer data is presented
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Release bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load and compile the vertex and fragment shaders
    unsigned int vertexShader;
    if (!compileShader(TUT2_VERTEX_SHADER, GL_VERTEX_SHADER, vertexShader))
    {
        // Error messages are printed inside compileShader
        return 1;
    }
    unsigned int fragmentShader;
    if (!compileShader(TUT2_FRAGMENT_SHADER, GL_FRAGMENT_SHADER, fragmentShader))
    {
        return 1;
    }

    // Link the compiled vertex and fragment shaders into a shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    {
        // Did the shader program link successfully? If not, find out why
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Failed to link shader program for some reason: " << std::endl << infoLog << std::endl;
            return 1;
        }
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Render loop - do not quit until I quit
    while (!glfwWindowShouldClose(window))
    {
        // Handle input
        processInput(window);

        // Render stuff
        glClearColor(0.0, 0.75, 1.0, 1.0); // Set clear colour in RGBA
        glClear(GL_COLOR_BUFFER_BIT); // Can also be GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT

        // Use shader program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // FINALLY DRAW THAT SHITE
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Display rendered frame while waiting for the other frame to render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Release all GLFW resources and exit
    glfwTerminate();
    return 0;
}

bool compileShader(const char* filename, unsigned int type, unsigned int &id)
{
    // Load vertex shader GLSL source and compile it
    const char* shaderType = "unused";
    switch(type)
    {
        case GL_VERTEX_SHADER:
            shaderType = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            shaderType = "fragment";
            break;
            /*
             // Only available in newer GL versions?
        case GL_TESS_CONTROL_SHADER:
            shaderType = "tesselation control";
            break;
        case GL_TESS_EVALUATION_SHADER:
            shaderType = "tesselation evaluation";
            break;
        case GL_COMPUTE_SHADER:
            shaderType = "compute";
            break;
            */
        case GL_GEOMETRY_SHADER:
            shaderType = "geometry";
            break;
    }

    // First, get file size
    struct stat vsStat;
    if (stat(filename, &vsStat) == -1) // POSIX system call
    {
        std::cerr << "Failed to load " << shaderType << " shader " << filename << std::endl;
        return false;
    }

    // Read the entire file
    int vertexShaderSize;
    if ((vsStat.st_mode & S_IFMT) == S_IFREG)
    {
        vertexShaderSize = vsStat.st_size;
    }
    else
    {
        std::cerr << shaderType << " shader " << filename << " is not a regular file!" << std::endl;
        return false;
    }
    char* vertexShaderSource = new char[vertexShaderSize];
    std::ifstream vertexShaderSourceStream(filename);
    vertexShaderSourceStream.read(vertexShaderSource, vertexShaderSize);

    // Associate source to shader and compile the shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(type);
    glShaderSource(vertexShader, 1, &vertexShaderSource, &vertexShaderSize);
    glCompileShader(vertexShader);
    delete[] vertexShaderSource;

    // Did compilation succeed?
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Cannot compile " << shaderType << " shader for some reason!" << std::endl << infoLog << std::endl;
        return false;
    }
    id = vertexShader;

    return true;
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