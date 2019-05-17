#include "glad.h"
#include "shader.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

// Static members are initialized outside of the constructor
KShaderProgram* KShaderProgram::currentProgram = nullptr;

KShaderProgram::KShaderProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath)
{
    usable = true;
    if (!compileShader(vertexShaderFilePath, GL_VERTEX_SHADER, vertexId))
    {
        usable = false;
        // Error messages are printed inside compileShader
    }
    if (!compileShader(fragmentShaderFilePath, GL_FRAGMENT_SHADER, fragmentId))
    {
        usable = false;
    }

    programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);

    int linkStatus;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus)
    {
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Failed to link shader program for some reason: " << std::endl << infoLog << std::endl;
        usable = false;
    }

    // Free shaders
    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
}

KShaderProgram::~KShaderProgram()
{
    if (currentProgram == this)
    {
        glUseProgram(0);
    }
}

bool KShaderProgram::compileShader(const char* filename, unsigned int type, unsigned int &id)
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
    int shaderSize;
    if ((vsStat.st_mode & S_IFMT) == S_IFREG)
    {
        shaderSize = vsStat.st_size;
    }
    else
    {
        std::cerr << shaderType << " shader " << filename << " is not a regular file!" << std::endl;
        return false;
    }
    char* shaderSource = new char[shaderSize];
    std::ifstream shaderSourceStream(filename);
    shaderSourceStream.read(shaderSource, shaderSize);

    // Associate source to shader and compile the shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(type);
    glShaderSource(vertexShader, 1, &shaderSource, &shaderSize);
    glCompileShader(vertexShader);
    delete[] shaderSource;

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

bool KShaderProgram::setVec4Uniform(const char* name, float x, float y, float z, float w)
{
    int uniformLocation = glGetUniformLocation(programId, name);
    if (uniformLocation >= 0)
    {
        glUniform4f(uniformLocation, x, y, z, w);
        return true;
    }
    return false;
}