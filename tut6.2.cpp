#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Use stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

// Tutorial 6: Coordinate systems - Part 3: Exercises
// https://learnopengl.com/Getting-started/Coordinate-Systems

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void processInput(GLFWwindow *window);
unsigned int loadImage(const char* fname, bool makeMipmap = true, GLint textureUnit = GL_TEXTURE0, GLint wrapMode = GL_REPEAT, GLint magFilter = GL_NEAREST, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR);
/*
const double PI = 3.14159265358979323846264338327950288419716939937510;
float degToRad(float degrees) { return degrees / (180 / PI); }
float radToDeg(float radians) { return radians * (180 / PI); }
*/
// void printMatrix(const KMatrix &mtx);

int main(int argc, char** argv) {

    int screenWidth = 800;
    int screenHeight = 600;

    // Set GLFW hints so that OpenGL version 3.3 is used
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Make the window
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Learn OpenGL", nullptr, nullptr);
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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)) );
    glEnableVertexAttribArray(1);

    // Release bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Release binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int texture = loadImage("dirbri18.png");
    unsigned int otherTex = loadImage("awesomeface.png", true, GL_TEXTURE1);

    glEnable(GL_DEPTH_TEST);

    // To be used later
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    std::cout << "========== CONTROLS ==========" << std::endl <<
    "Move around: HJKLWS (vim keys LOL)" << std::endl <<
    "Zoom in/out: T/Y" << std::endl <<
    "Expand vertically: up arrow" << std::endl <<
    "Shrink vertically: down arrow" << std::endl <<
    "Expand horizontally: right arrow" << std::endl <<
    "Shrink horizontally: left arrow" << std::endl <<
    "More coming soon..." << std::endl;

    {
        KShaderProgram theShader("tut6.vp", "tut6.fp");
        float xOffset = 0.;
        float yOffset = 0.;
        float zOffset = -3.;
        float fov = 45.0;
        float aspXfactor = 1.;
        float aspYfactor = 1.;
        /*
        unsigned int ctlRectIdxBuf[] = {
            0, 1, 3, 1, 2, 3
        };
        */
        // Render loop - do not quit until I quit
        while (!glfwWindowShouldClose(window))
        {
            // Handle input
            processInput(window);
            glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
            glViewport(0, 0, screenWidth, screenHeight);

            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
            {
                xOffset += .125;
            }
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            {
                yOffset += .125;
            }
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            {
                yOffset -= .125;
            }
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            {
                xOffset -= .125;
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                zOffset += .125;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                zOffset -= .125;
            }
            if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            {
                fov += .5;
            }
            if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
            {
                fov -= .5;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                aspYfactor -= 0.0625;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                aspYfactor += 0.0625;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                aspXfactor -= 0.0625;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                aspXfactor += 0.0625;
            }

            /*
            float ctlRectVBuf[] = {
            };
            */

            // Clear screen
            glClearColor(0.0, 0.75, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Global space to view coordinates
            glm::mat4 view(1.);
            view = glm::translate(view, glm::vec3(xOffset, yOffset, zOffset));

            // Projection
            glm::mat4 projection(1.);
            projection = glm::perspective(glm::radians(fov), ((float)screenWidth * aspXfactor) / ((float)screenHeight * aspYfactor), 0.1f, 100.f);

            // Use shader program
            theShader.use();
            theShader.setUniform("ourTexture", 0);
            theShader.setUniform("gratexture", 1);
            theShader.setUniform("view", view);
            theShader.setUniform("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, otherTex);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // FINALLY DRAW THAT SHITE
            for (int i = 0; i < 10; i++)
            {
                // Object-local to global space
                glm::mat4 model(1.);
                model = glm::translate(model, cubePositions[i]);
                float angle = glm::radians(20.0f * i);
                if (i % 3 == 0)
                {
                    angle += glfwGetTime();
                }
                model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));
                theShader.setUniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
            }
            // Draw controls

            // Display rendered frame while waiting for the other frame to render
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // Release all GLFW resources and exit
    glfwTerminate();
    return 0;
}

/*
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
*/
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

unsigned int loadImage(const char* fname, bool makeMipmap, GLint textureUnit, GLint wrapMode, GLint magFilter, GLint minFilter)
{
    // Load another texture
    int width, height, channels;
    unsigned char* data = stbi_load(fname, &width, &height, &channels, 0);
    unsigned int imageId;
    if (data)
    {
        // Create GL texture
        glGenTextures(1, &imageId);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, imageId);
        // Upload to GPU, set parameters, and generate mipmaps (lower res versions of the texture)
        int texFormat = channels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        if (makeMipmap)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        // Release bindings
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cerr << "Failed to load " << fname << " for some reason!" << std::endl;
        stbi_image_free(data);
        return 0;
    }
    stbi_image_free(data);
    return imageId;
}