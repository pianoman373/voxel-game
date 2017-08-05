#pragma once

#include <string>
#include "Math.hpp"

class Shader
{
private:
    // The program ID
    unsigned int id;

    std::string readShader(std::ifstream &file, std::string directory);

public:
    // Constructor reads and builds the shader
    Shader();

    void loadFile(const std::string vertexPath, const std::string fragmentPath);

    void load(const std::string vertex, const std::string fragment);

    // Use the program
    void bind();

    void uniformMat4(const std::string location, const mat4 &mat);

    void uniformVec3(const std::string location, const vec3 &vec);

    void uniformInt(const std::string location, int value);

    void uniformFloat(const std::string location, float value);

    void uniformBool(const std::string location, bool value);
};
