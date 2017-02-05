#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    // The program ID
    unsigned int id;
public:
    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use the program
    void bind();

    void uniform(const std::string location, const glm::mat4 &mat);

    void uniform(const std::string location, const glm::vec3 &vec);
};
