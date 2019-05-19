#pragma once

#include <unordered_map>

class KShaderProgram
{
protected:
    unsigned int programId;
    unsigned int vertexId;
    unsigned int fragmentId;
    static KShaderProgram *currentProgram;
    bool usable;
    // Map of names to uniforms
    std::unordered_map<const char*, int> uniformMap;

    bool compileShader(const char* source, unsigned int type, unsigned int &id);
public:
    KShaderProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
    ~KShaderProgram();

    bool use()
    {
        if (usable)
        {
            glUseProgram(programId);
            currentProgram = this;
        }
        return usable;
    }

    bool setUniform(const char* name, float x);
    bool setUniform(const char* name, float x, float y);
    bool setUniform(const char* name, float x, float y, float z);
    bool setUniform(const char* name, float x, float y, float z, float w);
    bool setUniform(const char* name, int x);
};