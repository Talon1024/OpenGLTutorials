#include "glad.h"

#include <iostream>
#include "shader.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>

// Tutorial 6: Coordinate systems - Part 3: Exercises
// Using SDL instead of GLFW
// https://learnopengl.com/Getting-started/Coordinate-Systems

bool sdlImage = false;

unsigned int loadGLImage(const char* fname, bool makeMipmap = true, GLint textureUnit = GL_TEXTURE0, GLint wrapMode = GL_REPEAT, GLint magFilter = GL_NEAREST, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR);
unsigned int SDLSurfaceToGLImage(SDL_Surface* surface, bool makeMipmap = true, GLint textureUnit = GL_TEXTURE0, GLint wrapMode = GL_REPEAT, GLint magFilter = GL_NEAREST, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR);
unsigned int tickCallback(unsigned int interval, void* param);
SDL_Surface* drawTextToSurface(const char* text, SDL_Surface* font, int cellSizeX = 8, int cellSizeY = 8);

struct tickParam
{
    unsigned int tick;
};

#define GL // Use OpenGL for rendering

int main(int argc, char** argv) {

    int screenWidth = 800;
    int screenHeight = 600;

    // Set GLFW hints so that OpenGL version 3.3 is used
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

#ifdef GL
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    // Make the window
    SDL_Window* window = SDL_CreateWindow(
        "Learn OpenGL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr)
    {
        std::cerr << "Failed to create SDL window" << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Failed to create SDL renderer" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

#ifdef GL
    // Initialize GLAD to manage OpenGL extensions
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // Set up viewport, and resize callback
    glViewport(0, 0, screenWidth, screenHeight);

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

    unsigned int texture = loadGLImage("dirbri18.png");
    unsigned int otherTex = loadGLImage("awesomeface.png", true, GL_TEXTURE1);

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
#endif
    if (!sdlImage)
    {
        if(!IMG_Init(IMG_INIT_PNG))
        {
            IMG_Quit();
            std::cout << SDL_GetError() << std::endl;
            return 1;
        }
        sdlImage = true;
    }

    SDL_Surface* font = IMG_Load("bitmapfont.png");
    SDL_Surface* controls = drawTextToSurface(
    "========== CONTROLS ==========\n"
    "Move around: HJKLWS (vim keys LOL)\n"
    "Zoom in/out: T/Y\n"
    "Expand vertically: Numpad 8\n"
    "Shrink vertically: Numpad 2\n"
    "Expand horizontally: Numpad 6\n"
    "Shrink horizontally: Numpad 4\n"
    "Turn: Arrow keys\n"
    "More coming soon...\n", font, 8, 8);

#ifdef GL
    unsigned int controlTexture = SDLSurfaceToGLImage(controls);
    float ctlVBuf[] = {
        -1.0, 1.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 1.0,
    };
    unsigned int ctlEBuf[] = {
        0, 1, 2,
        1, 2, 3,
    };

    unsigned int ctlVAO;
    glGenVertexArrays(1, &ctlVAO);
    glBindVertexArray(ctlVAO);

    unsigned int ctlVBO;
    glGenBuffers(1, &ctlVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ctlVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ctlVBuf), ctlVBuf, GL_STATIC_DRAW);

    unsigned int ctlEBO;
    glGenBuffers(1, &ctlEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctlEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ctlEBuf), ctlEBuf, GL_STATIC_DRAW);

    // Give OpenGL information about how the vertex buffer data is presented
    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)) );
    glEnableVertexAttribArray(1);

    // Release bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#else
    SDL_Texture* controlTexture = SDL_CreateTextureFromSurface(renderer, controls);
#endif
    {
#ifdef GL
        KShaderProgram theShader("tut6.vp", "tut6.fp");
        KShaderProgram shader2D("2d.vp", "2d.fp");
#endif
        float xOffset = 0.;
        float yOffset = 0.;
        float zOffset = -3.;
        float fov = 45.0;
        float aspXfactor = 1.;
        float aspYfactor = 1.;
        float yaw = 0.;
        float pitch = 0.;
        bool active = true;
        tickParam ticker = { 0 };
        SDL_TimerID tickerId = SDL_AddTimer(28, tickCallback, &ticker);
        // Render loop - do not quit until I quit
        while (active)
        {
#ifdef GL
            // Handle input
            glViewport(0, 0, screenWidth, screenHeight);

            // Clear screen
            glClearColor(0.0, 0.75, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Global space to view coordinates
            glm::mat4 view(1.);
            view = glm::rotate(view, yaw, glm::vec3(0., 1., 0.));
            view = glm::rotate(view, pitch, glm::vec3(1., 0., 0.));
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
                    angle += ticker.tick * .05;
                }
                model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));
                theShader.setUniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
            }

            shader2D.use();
            int um2TransformationLocation = shader2D.getUniformLocation("scale");
            float um2Scale[] = {
                (float)controls->w / screenWidth,
                (float)controls->h / screenHeight,
            };
            glUniform2fv(um2TransformationLocation, 1, um2Scale);
            um2TransformationLocation = shader2D.getUniformLocation("translate");
            float um2Translate[] = {
                (float)controls->w - 0,
                (float)controls->h - 0,
            };
            glUniform2fv(um2TransformationLocation, 1, um2Transformation);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, controlTexture);
            glBindVertexArray(ctlVAO);
            glBindBuffer(GL_ARRAY_BUFFER, ctlVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctlEBO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
#else
            SDL_Rect destRect { 0, 0, controls->w, controls->h };
            SDL_RenderCopy(renderer, controlTexture, nullptr, &destRect);
#endif
            // Display rendered frame while waiting for the other frame to render
            SDL_GL_SwapWindow(window);
            SDL_Event event;
            SDL_PollEvent(&event);
            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
                }
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    active = false;
                    SDL_RemoveTimer(tickerId);
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Negative powers of 2 are easy on a computer
                if (event.key.keysym.sym == SDLK_h)
                {
                    xOffset += .125;
                }
                if (event.key.keysym.sym == SDLK_j)
                {
                    yOffset += .125;
                }
                if (event.key.keysym.sym == SDLK_k)
                {
                    yOffset -= .125;
                }
                if (event.key.keysym.sym == SDLK_l)
                {
                    xOffset -= .125;
                }
                if (event.key.keysym.sym == SDLK_w)
                {
                    zOffset += .125;
                }
                if (event.key.keysym.sym == SDLK_s)
                {
                    zOffset -= .125;
                }
                if (event.key.keysym.sym == SDLK_t)
                {
                    fov += .5;
                }
                if (event.key.keysym.sym == SDLK_y)
                {
                    fov -= .5;
                }
                if (event.key.keysym.sym == SDLK_KP_8)
                {
                    aspYfactor -= 0.0625;
                }
                if (event.key.keysym.sym == SDLK_KP_2)
                {
                    aspYfactor += 0.0625;
                }
                if (event.key.keysym.sym == SDLK_KP_6)
                {
                    aspXfactor -= 0.0625;
                }
                if (event.key.keysym.sym == SDLK_KP_4)
                {
                    aspXfactor += 0.0625;
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    pitch += .03125;
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    pitch -= .03125;
                }
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    yaw += .03125;
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    yaw -= .03125;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    active = false;
                    SDL_RemoveTimer(tickerId);
                }
            }
        }
#ifdef GL
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }

#ifndef GL
    SDL_DestroyTexture(controlTexture);
#else
    SDL_GL_DeleteContext(glcontext);
#endif
    // Release all GLFW resources and exit
    if(sdlImage)
    {
        IMG_Quit();
    }
    SDL_FreeSurface(font);
    SDL_FreeSurface(controls);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

unsigned int loadGLImage(const char* fname, bool makeMipmap, GLint textureUnit, GLint wrapMode, GLint magFilter, GLint minFilter)
{
    // Load another texture
    unsigned int imageId;
    if (!sdlImage)
    {
        sdlImage = IMG_Init(IMG_INIT_PNG) != 0;
        if (!sdlImage)
        {
            IMG_Quit();
            return 0;
        }
    }
    SDL_Surface* imgSurface = IMG_Load(fname);
    if (imgSurface)
    {
        imageId = SDLSurfaceToGLImage(imgSurface, makeMipmap, textureUnit, wrapMode, magFilter, minFilter);
        SDL_FreeSurface(imgSurface);
        return imageId;
    }
    std::cerr << "Failed to load " << fname << " for some reason!" << std::endl;
    return 0;
}

unsigned int SDLSurfaceToGLImage(SDL_Surface* surface, bool makeMipmap, GLint textureUnit, GLint wrapMode, GLint magFilter, GLint minFilter)
{
    unsigned int imageId;
    if (surface)
    {
        if (surface->format->format != SDL_PIXELFORMAT_RGB24 && 
            surface->format->format != SDL_PIXELFORMAT_RGBA32)
        {
            SDL_PixelFormat GLSDLPixelFormat;
            if (surface->format->format == SDL_PIXELFORMAT_INDEX1LSB ||
                surface->format->format == SDL_PIXELFORMAT_INDEX1MSB ||
                surface->format->format == SDL_PIXELFORMAT_INDEX4LSB ||
                surface->format->format == SDL_PIXELFORMAT_INDEX4MSB ||
                surface->format->format == SDL_PIXELFORMAT_INDEX8 ||
                surface->format->format == SDL_PIXELFORMAT_RGB332 ||
                surface->format->format == SDL_PIXELFORMAT_RGB444 ||
                surface->format->format == SDL_PIXELFORMAT_RGB555 ||
                surface->format->format == SDL_PIXELFORMAT_RGB565 ||
                surface->format->format == SDL_PIXELFORMAT_RGB24 ||
                surface->format->format == SDL_PIXELFORMAT_RGB888 ||
                surface->format->format == SDL_PIXELFORMAT_BGR555 ||
                surface->format->format == SDL_PIXELFORMAT_BGR565 ||
                surface->format->format == SDL_PIXELFORMAT_BGR24 ||
                surface->format->format == SDL_PIXELFORMAT_BGR888)
            {
                GLSDLPixelFormat.format = SDL_PIXELFORMAT_RGB24;
            }
            else
            {
                GLSDLPixelFormat.format = SDL_PIXELFORMAT_RGBA32;
            }
            GLSDLPixelFormat.palette = nullptr;
            GLSDLPixelFormat.BitsPerPixel = 8;
            GLSDLPixelFormat.BytesPerPixel = 4;
            GLSDLPixelFormat.Rmask =
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0xFF000000;
#else
            0x000000FF;
#endif
            GLSDLPixelFormat.Gmask =
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0x00FF0000;
#else
            0x0000FF00;
#endif
            GLSDLPixelFormat.Bmask =
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0x0000FF00;
#else
            0x00FF0000;
#endif
            GLSDLPixelFormat.Amask =
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0x000000FF;
#else
            0xFF000000;
#endif
            SDL_ConvertSurface(surface, &GLSDLPixelFormat, 0);
        }
        // Create GL texture
        glGenTextures(1, &imageId);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, imageId);
        // Upload to GPU, set parameters, and generate mipmaps (lower res versions of the texture)
        int texFormat = surface->format->format == SDL_PIXELFORMAT_RGB24 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, texFormat, surface->w, surface->h, 0, texFormat, GL_UNSIGNED_BYTE, surface->pixels);
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
        return 0;
    }
    return imageId;
}

unsigned int tickCallback(unsigned int interval, void* param)
{
    tickParam* ticker = (tickParam*)param;
    ticker->tick += 1;
    return interval;
}

SDL_Surface* drawTextToSurface(const char* text, SDL_Surface* font, int cellSizeX, int cellSizeY)
{
    int textSurfaceWidth = 0;
    int textSurfaceHeight = 0;
    int i = 0;
    int linePos = 0;
    int lineCount = 1;
    // Assume null-terminated string
    while (text[i] != 0)
    {
        linePos++;
        if (text[i] == '\n')
        {
            lineCount++;
            if (linePos * cellSizeX > textSurfaceWidth)
            {
                textSurfaceWidth = linePos * cellSizeX;
            }
            linePos = 0;
        }
        ++i;
    }
    textSurfaceHeight = lineCount * cellSizeY;
    SDL_Surface* textSurface = SDL_CreateRGBSurface(
        0,
        textSurfaceWidth,
        textSurfaceHeight,
        32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xFF000000,
        0x00FF0000,
        0x0000FF00,
        0x000000FF
#else
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
#endif
    );
    i = 0;
    linePos = 0;
    lineCount = 0;
    while (text[i] != 0)
    {
        if(text[i] == '\n')
        {
            linePos = 0;
            lineCount++;
            i++;
            continue;
        }
        int fontCharX = text[i] * cellSizeX;
        int fontCharY = (fontCharX / font->w) * cellSizeY;
        fontCharX %= font->w;
        SDL_Rect surfaceRect { linePos * cellSizeX, lineCount * cellSizeY, cellSizeX, cellSizeY };
        SDL_Rect fontCharRect { fontCharX, fontCharY, cellSizeX, cellSizeY };
        SDL_BlitSurface(font, &fontCharRect, textSurface, &surfaceRect);
        linePos++;
        i++;
    }
    return textSurface;
}