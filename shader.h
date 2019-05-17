#pragma once

class KShaderProgram
{
protected:
    unsigned int programId;
    unsigned int vertexId;
    unsigned int fragmentId;
    static KShaderProgram *currentProgram;
    bool usable;

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
    bool setVec4Uniform(const char* name, float x, float y, float z, float w);
};