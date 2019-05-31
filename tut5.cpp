#include "glad.h"
#include <GLFW/glfw3.h>
#include "config.h"
#include <iostream>
#include "shader.h"
#include <cmath>
#include "kmatrix.h"

// Use stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

// Tutorial 5: Transformations
// https://learnopengl.com/Getting-started/Transformations

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void processInput(GLFWwindow *window);

const double PI = 3.14159265358979323846264338327950288419716939937510;
float degToRad(float degrees) { return degrees / (180 / PI); }
float radToDeg(float radians) { return radians * (180 / PI); }
void printMatrix(const KMatrix &mtx);

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
    // Define vertex array
    float vertices[] = {
        // positions     // colours     // uvs
         0.0,  0.5, 0.0, 1.0, 0.0, 0.0, 0.5, 0.0,  // top right
         0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, // bottom right
        -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, // bottom left
        // -0.5,  0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        // 1, 2, 3    // second triangle
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
    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // Vertex colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)) );
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)) );
    glEnableVertexAttribArray(2);

    // Release bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Make element buffer
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Release binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Load texture image
    int width, height, channels;
    unsigned char* data = stbi_load("dirbri18.png", &width, &height, &channels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Release bindings
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cerr << "Failed to load dirbri18.png for some reason!" << std::endl;
    }
    // Free texture from memory
    stbi_image_free(data);
    
    KMatrix identity = KMatrix::Identity();
    KMatrix trans = KMatrix::Translation(1, 1, 0);
    KMatrix scale = KMatrix::Scale(.5, .5, .5);
    KMatrix rot = KMatrix::Rotation(degToRad(90), 0, 0);
    KMatrix transform = trans * scale * rot;

    std::cout << "Identity matrix" << std::endl;
    printMatrix(identity);
    std::cout << "Translation matrix" << std::endl;
    printMatrix(trans);
    std::cout << "Scale matrix" << std::endl;
    printMatrix(scale);
    std::cout << "Rotation matrix" << std::endl;
    printMatrix(rot);
    std::cout << "Combined transformation" << std::endl;
    printMatrix(transform);

    {
        KShaderProgram theShader("tut5.vp", "tut4.fp");
        // Render loop - do not quit until I quit
        while (!glfwWindowShouldClose(window))
        {
            // Handle input
            processInput(window);

            // Render stuff
            glClearColor(0.0, 0.75, 1.0, 1.0); // Set clear colour in RGBA
            glClear(GL_COLOR_BUFFER_BIT); // Can also be GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT

            // Use shader program
            theShader.use();
            float timeValue = glfwGetTime();
            transform = KMatrix::Translation(0, std::sin(timeValue), 0);
            theShader.setUniform("transform", transform);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // FINALLY DRAW THAT SHITE
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

            // Display rendered frame while waiting for the other frame to render
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // Release all GLFW resources and exit
    glfwTerminate();
    return 0;
}

void printMatrix(const KMatrix &mtx)
{
    for (unsigned int row = 0; row < mtx.GetRows(); row++)
    {
        for (unsigned int col = 0; col < mtx.GetCols(); col++)
        {
            std::cout << mtx.GetEntry(row, col) << "  ";
        }
        std::cout << std::endl;
    }
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