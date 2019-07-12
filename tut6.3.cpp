#include <iostream>
#include <cstring>
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

#include "glad.h"
#include "input.h"
#include "shader.h"

// Tutorial 6: Coordinate systems - Part 3: Exercises
// Using SDL instead of GLFW
// https://learnopengl.com/Getting-started/Coordinate-Systems

bool sdlImage = false;

unsigned int loadGLImage(const char* fname, bool makeMipmap = true, GLint textureUnit = GL_TEXTURE0, GLint wrapMode = GL_REPEAT, GLint magFilter = GL_NEAREST, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR);
unsigned int SDLSurfaceToGLImage(SDL_Surface*& surface, bool makeMipmap = true, GLint textureUnit = GL_TEXTURE0, GLint wrapMode = GL_REPEAT, GLint magFilter = GL_NEAREST, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR);
unsigned int tickCallback(unsigned int interval, void* param);
SDL_Surface* drawTextToSurface(const char* text, SDL_Surface* font, int cellSizeX = 8, int cellSizeY = 8);

struct controlledCamera
{
    float xOffset = 0.;
    float yOffset = 0.;
    float zOffset = -3.;
    float fov = 45.0;
    float aspXfactor = 1.;
    float aspYfactor = 1.;
    float yaw = 0.;
    float pitch = 0.;
    int inXOffset = 0;
    int inYOffset = 0;
    int inZOffset = 0;
    int inFov = 0;
    int inAspXFactor = 0;
    int inAspYFactor = 0;
    int inYaw = 0;
    int inPitch = 0;
};

struct tickParam
{
    unsigned int tick;
    controlledCamera* camera;
};

template<typename T> struct vector2
{
    T x;
    T y;
};

vector2<unsigned int> getTextGridSize(const char* text);

struct QuadGrid {
    float* pos;
    float* uv;
    unsigned int* el;
    // unsigned int elementCount;
    unsigned int rows;
    unsigned int cols;
    QuadGrid(unsigned int rows, unsigned int cols) : rows(rows), cols(cols)
    {
        // 0   1
        // +---+
        // |  /|
        // | / |
        // |/  |
        // +---+
        // 2   3
        // 0 1 2 1 2 3
        float xFactor[] = { 0.0, 1.0, 0.0, 1.0 };
        float yFactor[] = { 0.0, 0.0, 1.0, 1.0 };
        unsigned int elements[] = { 0, 1, 2, 1, 2, 3 };
        if (rows > 0 && cols > 0)
        {
            unsigned int quadCount = rows * cols;
            // 4 vertices per quad, 2 values per vertex
            pos = new float[quadCount * 8];
            uv = new float[quadCount * 8];
            // 3 vertices per triangle, 2 triangles per quad
            el = new unsigned int[quadCount * 6];
            unsigned int curVertex = 0;
            unsigned int curElement = 0;
            float xSize = 1. / cols * 2;
            float ySize = 1. / rows * 2;
            for (unsigned int row = 0; row < rows; row++)
            {
                for (unsigned int col = 0; col < cols; col++)
                {
                    float xPos = (float)col / cols * 2 - 1;
                    float yPos = 1 - (float)row / rows * 2;
                    for (unsigned int element = 0; element < 6; element++)
                    {
                        el[curElement++] = elements[element] + curVertex;
                    }
                    for (unsigned int vertex = 0; vertex < 4; vertex++)
                    {
                        pos[curVertex * 2] = xPos + xSize * xFactor[vertex];
                        pos[curVertex * 2 + 1] = yPos + ySize * yFactor[vertex];
                        uv[curVertex * 2] = xFactor[vertex];
                        uv[curVertex * 2 + 1] = yFactor[vertex];
                        curVertex++;
                    }
                }
            }
            // elementCount = curElement;
        }
    }
};

class FontTexture {
private:
    vector2<unsigned int> imageSize;
    vector2<unsigned int> cellSize;
    // Size of each cell as a percentage of the width/height
    vector2<float> cellUv;
    unsigned int textureId;
public:
    FontTexture(SDL_Surface*& texture, vector2<unsigned int> cellSize, int texUnit = 0)
    {
        imageSize.x = texture->w;
        imageSize.y = texture->h;
        this->cellSize = cellSize;
        cellUv.x = (float)cellSize.x / imageSize.x;
        cellUv.y = (float)cellSize.y / imageSize.y;
        textureId = SDLSurfaceToGLImage(texture, true, texUnit);
        if (textureId == 0)
        {
            std::cerr << "Failed to convert the texture for some reason!" << std::endl;
        }
    }
    ~FontTexture()
    {
        glDeleteTextures(1, &textureId);
    }
    // Delete copy constructor and copy assignment constructor
    FontTexture(const FontTexture&) = delete;
    FontTexture& operator= (const FontTexture&) = delete;
    // Move and move assignment constructors
    FontTexture(FontTexture&& previous)
    {
        this->imageSize = previous.imageSize;
        this->cellSize = previous.cellSize;
        this->cellUv = previous.cellUv;
        this->textureId = previous.textureId;
    }
    FontTexture& operator= (FontTexture&& previous);
    // UV coordinates for upper left corner of the given byte
    vector2<float> uvForChar(char ch) const;
    vector2<float> getCellUv() const
    {
        return cellUv;
    }
    vector2<unsigned int> getCellSize() const
    {
        return cellSize;
    }
    unsigned int getTextureId() const
    {
        return textureId;
    }
};

FontTexture& FontTexture::operator= (FontTexture&& previous)
{
    this->imageSize = previous.imageSize;
    this->cellSize = previous.cellSize;
    this->cellUv = previous.cellUv;
    this->textureId = previous.textureId;
    return *this;
}

vector2<float> FontTexture::uvForChar(char ch) const
{
    // Get the UV for the upper left corner of a particular character
    vector2<unsigned int> cells;
    vector2<unsigned int> pos;
    cells.x = imageSize.x / cellSize.x;
    cells.y = imageSize.y / cellSize.y;
    pos.x = ch % cells.y;
    pos.y = ch / cells.x;
    vector2<float> uv;
    uv.x = cellUv.x * pos.x;
    uv.y = cellUv.y * pos.y;
    return uv;
}

void drawTextOnQuadGrid(const char* text, const FontTexture& fontexture, QuadGrid& grid);

#define GL // Use OpenGL for rendering
#define CUBES

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

#ifdef CUBES
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
    "Zoom in/out: Y/T\n"
    "Expand vertically: Numpad 8\n"
    "Shrink vertically: Numpad 2\n"
    "Expand horizontally: Numpad 6\n"
    "Shrink horizontally: Numpad 4\n"
    "Turn: Arrow keys\n"
    "More coming soon...\n", font, 8, 8);
#ifdef GL
    unsigned int controlTexture = SDLSurfaceToGLImage(controls);
    float ctlVBuf[] = {
        -1., 1., 0.0, 0.0,
        1., 1., 1.0, 0.0,
        -1., -1., 0.0, 1.0,
        1., -1., 1.0, 1.0,
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

    FontTexture fontTexture(font, {8, 8}, GL_TEXTURE1);
    unsigned int fontTextureId = fontTexture.getTextureId();
    const char* stTextFmt = "========== STATS ==========\n"
        "X Offset: %0.4f\n"
        "Y Offset: %0.4f\n"
        "Z Offset: %0.4f\n"
        "FOV: %0.4f degrees\n"
        "Aspect Ratio X: %0.4f\n"
        "Aspect Ratio Y: %0.4f\n"
        "Yaw: %0.4f\n"
        "Pitch: %0.4f\n";
    vector2<unsigned int> stCells = getTextGridSize(stTextFmt);
    char* stText = new char[stCells.x * stCells.y];
    std::cout << "stCells " << stCells.x << " " << stCells.y << std::endl;
    QuadGrid stQuad(stCells.y, stCells.x);
    drawTextOnQuadGrid(stTextFmt, fontTexture, stQuad);

    unsigned int stVAO;
    glGenVertexArrays(1, &stVAO);
    glBindVertexArray(stVAO);

    unsigned int stData[] = {0, 0, 0};
    unsigned int &stPosVBO = stData[0];
    unsigned int &stUvVBO = stData[1];
    unsigned int &stEBO = stData[2];
    glGenBuffers(3, stData);
    glBindBuffer(GL_ARRAY_BUFFER, stPosVBO);
    glBufferData(GL_ARRAY_BUFFER, stQuad.rows * stQuad.cols * sizeof(float) * 8, stQuad.pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, stUvVBO);
    glBufferData(GL_ARRAY_BUFFER, stQuad.rows * stQuad.cols * sizeof(float) * 8, stQuad.uv, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, stQuad.rows * stQuad.cols * sizeof(unsigned int) * 6, stQuad.el, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#else
    SDL_Texture* controlTexture = SDL_CreateTextureFromSurface(renderer, controls);
#endif
    {
#ifdef GL
#ifdef CUBES
        KShaderProgram theShader("tut6.vp", "tut6.fp");
#endif
        KShaderProgram shader2D("2d.vp", "2d.fp");
#endif
        controlledCamera camera; //{0., 0., -3., 45., 1., 1., 0., 0., 0, 0, 0, 0, 0, 0, 0, 0};

        bool active = true;
        KeyboardInputHandler kbInput;
        kbInput.setHandler(SDLK_h, true, [&camera]{ camera.inXOffset = 1; });
        kbInput.setHandler(SDLK_h, false, [&camera]{ camera.inXOffset = 0; });
        kbInput.setHandler(SDLK_j, true, [&camera]{ camera.inYOffset = 1; });
        kbInput.setHandler(SDLK_j, false, [&camera]{ camera.inYOffset = 0; });
        kbInput.setHandler(SDLK_k, true, [&camera]{ camera.inYOffset = -1; });
        kbInput.setHandler(SDLK_k, false, [&camera]{ camera.inYOffset = 0; });
        kbInput.setHandler(SDLK_l, true, [&camera]{ camera.inXOffset = -1; });
        kbInput.setHandler(SDLK_l, false, [&camera]{ camera.inXOffset = 0; });
        kbInput.setHandler(SDLK_w, true, [&camera]{ camera.inZOffset = 1; });
        kbInput.setHandler(SDLK_w, false, [&camera]{ camera.inZOffset = 0; });
        kbInput.setHandler(SDLK_s, true, [&camera]{ camera.inZOffset = -1; });
        kbInput.setHandler(SDLK_s, false, [&camera]{ camera.inZOffset = 0; });
        kbInput.setHandler(SDLK_t, true, [&camera]{ camera.inFov = 1; });
        kbInput.setHandler(SDLK_t, false, [&camera]{ camera.inFov = 0; });
        kbInput.setHandler(SDLK_y, true, [&camera]{ camera.inFov = -1; });
        kbInput.setHandler(SDLK_y, false, [&camera]{ camera.inFov = 0; });
        kbInput.setHandler(SDLK_KP_8, true, [&camera]{ camera.inAspYFactor = -1; });
        kbInput.setHandler(SDLK_KP_8, false, [&camera]{ camera.inAspYFactor = 0; });
        kbInput.setHandler(SDLK_KP_2, true, [&camera]{ camera.inAspYFactor = 1; });
        kbInput.setHandler(SDLK_KP_2, false, [&camera]{ camera.inAspYFactor = 0; });
        kbInput.setHandler(SDLK_KP_6, true, [&camera]{ camera.inAspXFactor = -1; });
        kbInput.setHandler(SDLK_KP_6, false, [&camera]{ camera.inAspXFactor = 0; });
        kbInput.setHandler(SDLK_KP_4, true, [&camera]{ camera.inAspXFactor = 1; });
        kbInput.setHandler(SDLK_KP_4, false, [&camera]{ camera.inAspXFactor = 0; });
        kbInput.setHandler(SDLK_UP, true, [&camera]{ camera.inPitch = 1; });
        kbInput.setHandler(SDLK_UP, false, [&camera]{ camera.inPitch = 0; });
        kbInput.setHandler(SDLK_DOWN, true, [&camera]{ camera.inPitch = -1; });
        kbInput.setHandler(SDLK_DOWN, false, [&camera]{ camera.inPitch = 0; });
        kbInput.setHandler(SDLK_RIGHT, true, [&camera]{ camera.inYaw = 1; });
        kbInput.setHandler(SDLK_RIGHT, false, [&camera]{ camera.inYaw = 0; });
        kbInput.setHandler(SDLK_LEFT, true, [&camera]{ camera.inYaw = -1; });
        kbInput.setHandler(SDLK_LEFT, false, [&camera]{ camera.inYaw = 0; });

        tickParam ticker = { 0, &camera };
        SDL_TimerID tickerId = SDL_AddTimer(30, tickCallback, &ticker);
        // Render loop - do not quit until I quit
        while (active)
        {
#ifdef GL
            // Handle input
            glViewport(0, 0, screenWidth, screenHeight);

            // Clear screen
            glClearColor(0.0, 0.75, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef CUBES
            // Global space to view coordinates
            glm::mat4 view(1.);
            view = glm::rotate(view, camera.yaw, glm::vec3(0., 1., 0.));
            view = glm::rotate(view, camera.pitch, glm::vec3(1., 0., 0.));
            view = glm::translate(view, glm::vec3(camera.xOffset, camera.yOffset, camera.zOffset));

            // Projection
            glm::mat4 projection(1.);
            projection = glm::perspective(glm::radians(camera.fov), ((float)screenWidth * camera.aspXfactor) / ((float)screenHeight * camera.aspYfactor), 0.1f, 100.f);

            // Use shader program
            theShader.use();
            theShader.setUniform("ourTexture", 0);
            theShader.setUniform("gratexture", 1);
            theShader.setUniform("view", view);
            theShader.setUniform("projection", projection);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
#endif
            shader2D.use();
            int uv2ScaleLocation = shader2D.getUniformLocation("scale");
            float uv2Scale[] = {
                (float)controls->w / screenWidth,
                (float)controls->h / screenHeight,
            };
            //uv2Scale[0] = 1;
            //uv2Scale[1] = 1;
            glUniform2fv(uv2ScaleLocation, 1, uv2Scale);
            int uv2TranslateLocation = shader2D.getUniformLocation("translate");
            float uv2Translate[] = {
                -1 + uv2Scale[0],
                1 - uv2Scale[1],
            };
            //uv2Translate[0] = 0;
            //uv2Translate[1] = 0;
            glUniform2fv(uv2TranslateLocation, 1, uv2Translate);
            shader2D.setUniform("theTexture", 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, controlTexture);
            glBindVertexArray(ctlVAO);
            glBindBuffer(GL_ARRAY_BUFFER, ctlVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctlEBO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            shader2D.use();
            std::snprintf(stText, stCells.x * stCells.y, stTextFmt, camera.xOffset, camera.yOffset, camera.zOffset, camera.fov, camera.aspXfactor, camera.aspYfactor, camera.yaw, camera.pitch);
            drawTextOnQuadGrid(stText, fontTexture, stQuad);
            glBindBuffer(GL_ARRAY_BUFFER, stUvVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, stQuad.rows * stQuad.cols * sizeof(unsigned int) * 6, stQuad.uv);

            uv2Scale[0] = (float)(stCells.x * fontTexture.getCellSize().x) / screenWidth;
            uv2Scale[1] = (float)(stCells.y * fontTexture.getCellSize().y) / screenHeight;
            glUniform2fv(uv2ScaleLocation, 1, uv2Scale);
            uv2Translate[0] = 1 - uv2Scale[0] * 2;
            uv2Translate[1] = 1 - uv2Scale[1] * 2;
            glUniform2fv(uv2TranslateLocation, 1, uv2Translate);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, fontTextureId);
            shader2D.setUniform("theTexture", 1);
            glBindVertexArray(stVAO);
            glBindBuffer(GL_ARRAY_BUFFER, stPosVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stEBO);
            glDrawElements(GL_TRIANGLES, stQuad.rows * stQuad.cols * 6, GL_UNSIGNED_INT, 0);
#else
            SDL_Rect destRect { 0, 0, controls->w, controls->h };
            SDL_RenderCopy(renderer, controlTexture, nullptr, &destRect);
#endif
            // Display rendered frame while waiting for the other frame to render
            SDL_GL_SwapWindow(window);
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
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
                else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
                {
                    bool state = event.type == SDL_KEYDOWN;
                    if (kbInput.handle(event.key.keysym.sym, state))
                    {
                        continue;
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        active = false;
                        SDL_RemoveTimer(tickerId);
                    }
                }
            }
        }
#ifdef GL
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }

    delete[] stText;
#ifndef GL
    SDL_DestroyTexture(controlTexture);
#else
    SDL_GL_DeleteContext(glcontext);
    delete[] stQuad.pos;
    delete[] stQuad.uv;
    delete[] stQuad.el;
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

unsigned int tickCallback(unsigned int interval, void* param)
{
    tickParam* ticker = (tickParam*)param;
    ticker->tick += 1;
    ticker->camera->xOffset += .125 * ticker->camera->inXOffset;
    ticker->camera->yOffset += .125 * ticker->camera->inYOffset;
    ticker->camera->zOffset += .125 * ticker->camera->inZOffset;
    ticker->camera->fov += .5 * ticker->camera->inFov;
    ticker->camera->aspXfactor += 0.0625 * ticker->camera->inAspXFactor;
    ticker->camera->aspYfactor += 0.0625 * ticker->camera->inAspYFactor;
    ticker->camera->pitch += .03125 * ticker->camera->inPitch;
    ticker->camera->yaw += .03125 * ticker->camera->inYaw;
    return interval;
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

unsigned int SDLSurfaceToGLImage(SDL_Surface*& surface, bool makeMipmap, GLint textureUnit, GLint wrapMode, GLint magFilter, GLint minFilter)
{
    unsigned int imageId;
    if (surface)
    {
        // Convert surface pixel format to a format usable by OpenGL
        if (surface->format->format != SDL_PIXELFORMAT_RGB24 &&
            surface->format->format != SDL_PIXELFORMAT_RGBA32)
        {
            unsigned int newPixelFormat = 0;
            if (SDL_ISPIXELFORMAT_ALPHA(surface->format->format))
            {
                newPixelFormat = SDL_PIXELFORMAT_RGBA32;
            }
            else
            {
                newPixelFormat = SDL_PIXELFORMAT_RGB24;
            }
            if (surface->format->format != newPixelFormat)
            {
                SDL_Surface* newSurface = SDL_ConvertSurfaceFormat(surface, newPixelFormat, 0);
                if (newSurface == NULL)
                {
                    std::cerr << "Failed to convert the surface for some reason:" << std::endl <<
                        SDL_GetError() << std::endl;
                    return 0;
                }
                else
                {
                    SDL_FreeSurface(surface);
                    surface = newSurface;
                }
            }
        }
        // Create GL texture
        glGenTextures(1, &imageId);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, imageId);
        // Upload to GPU, set parameters, and generate mipmaps (lower res versions of the texture)
        int texFormat = surface->format->format == SDL_PIXELFORMAT_RGB24 ? GL_RGB : GL_RGBA;
        /*
        std::cout << "texFormat is ";
        if (texFormat == GL_RGB)
        {
            std::cout << "GL_RGB";
        }
        else
        {
            std::cout << "GL_RGBA";
        }
        */
        std::cout << std::endl;
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

vector2<unsigned int> getTextGridSize(const char* text)
{
    vector2<unsigned int> gridSize {0, 0};
    unsigned int textIndex = 0;
    unsigned int linePos = 0;
    // Assume null-terminated string
    while (text[textIndex] != 0)
    {
        linePos++;
        if (text[textIndex] == '\n')
        {
            gridSize.y++;
            if (linePos > gridSize.x)
            {
                gridSize.x = linePos;
            }
            linePos = 0;
        }
        ++textIndex;
    }
    // std::cout << "Length of text: " << textIndex << std::endl;
    return gridSize;
}

SDL_Surface* drawTextToSurface(const char* text, SDL_Surface* font, int cellSizeX, int cellSizeY)
{
    vector2<unsigned int> gridSize = getTextGridSize(text);
    // std::cout << "gridSize " << gridSize.x << " " << gridSize.y << std::endl;
    int textSurfaceWidth = gridSize.x * cellSizeX;
    int textSurfaceHeight = gridSize.y * cellSizeY;
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
    unsigned int textIndex = 0;
    int linePos = 0;
    int lineCount = 0;
    while (text[textIndex] != 0)
    {
        if(text[textIndex] == '\n')
        {
            linePos = 0;
            lineCount++;
            textIndex++;
            continue;
        }
        int fontCharX = text[textIndex] * cellSizeX;
        int fontCharY = (fontCharX / font->w) * cellSizeY;
        fontCharX %= font->w;
        SDL_Rect surfaceRect { linePos * cellSizeX, lineCount * cellSizeY, cellSizeX, cellSizeY };
        SDL_Rect fontCharRect { fontCharX, fontCharY, cellSizeX, cellSizeY };
        SDL_BlitSurface(font, &fontCharRect, textSurface, &surfaceRect);
        linePos++;
        textIndex++;
    }
    return textSurface;
}

void drawTextOnQuadGrid(const char* text, const FontTexture& fontexture, QuadGrid& grid)
{
    // 0   1
    // +---+
    // |  /|
    // | / |
    // |/  |
    // +---+
    // 2   3
    // 0 1 2 1 2 3
    float xFactor[] = { 0.0, 1.0, 0.0, 1.0 };
    float yFactor[] = { 0.0, 0.0, 1.0, 1.0 };
    unsigned int row = 0, col = 0, textIndex = 0;
    bool onText = true;
    char curChar = text[textIndex];
    vector2<float> cellUv = fontexture.getCellUv();
    while (curChar != 0)
    {
        /*
        if (onText)
        {
            std::printf("%hhu\n", curChar);
        }
        */
        vector2<float> uv = fontexture.uvForChar(curChar);
        unsigned int previous = row * grid.cols * 4 + col * 4;
        for (unsigned int current = 0; current < 4; current++)
        {
            unsigned int vertex = previous + current;
            if (onText)
            {
                grid.uv[vertex * 2] = uv.x + cellUv.x * xFactor[current];
                grid.uv[vertex * 2 + 1] = uv.y + cellUv.y * (1 - yFactor[current]);
            }
            else
            {
                grid.uv[vertex * 2] = 0.;
                grid.uv[vertex * 2 + 1] = 0.;
            }
        }
        col++;
        if (curChar == '\n')
        {
            onText = false;
        }
        if (col == grid.cols)
        {
            col = 0;
            row++;
            while (curChar != '\n')
            {
                curChar = text[++textIndex];
            }
            onText = true;
        }
        if (onText)
        {
            curChar = text[++textIndex];
        }
    }
}