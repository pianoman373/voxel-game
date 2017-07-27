#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>

Shader::Shader() {

}

void Shader::load(const std::string vertexPath, const std::string fragmentPath) {
    // 1. Retrieve the vertex/fragment source code from filePath

    std::cout << "loading shaders: " << vertexPath << " and " << fragmentPath << std::endl;

    std::string directory = vertexPath.substr(0, vertexPath.find_last_of("/\\"));

    std::ifstream vertexStream(vertexPath);
    std::ifstream fragmentStream(fragmentPath);

    std::string vertexCode = readShader(vertexStream, directory);
    std::string fragmentCode = readShader(fragmentStream, directory);

    vertexStream.close();
    fragmentStream.close();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //shaders
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    this->id = glCreateProgram();

    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);
    glLinkProgram(this->id);

    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(this->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

std::string Shader::readShader(std::ifstream &file, std::string directory) {
    std::string source, line;
    while (std::getline(file, line))
    {
        std::string prefix = "#include \"";
        if(line.substr(0, prefix.size()) == prefix) {
            //::cout << "found include" << std::endl;

            if (line.substr(line.size() - 1) == "\"") {
                //std::cout << line.substr(prefix.size(), (line.size() - 1) - prefix.size()) << std::endl;

                std::string includePath = directory + "/" + line.substr(prefix.size(), (line.size() - 1) - prefix.size());
                std::ifstream includeFile(includePath);
                if (includeFile.is_open())
                {
                    source += readShader(includeFile, directory);
                }
                includeFile.close();
            }
        }
        else {
            source += line + "\n";
        }
    }
    return source;
}

void Shader::bind() {
    glUseProgram(this->id);
}

void Shader::uniformMat4(const std::string location, const mat4 &mat) {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());

    float matrixArray[] = {
        mat.m00, mat.m10, mat.m20, mat.m30,
        mat.m01, mat.m11, mat.m21, mat.m31,
        mat.m02, mat.m12, mat.m22, mat.m32,
        mat.m03, mat.m13, mat.m23, mat.m33
    };
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matrixArray);
}

void Shader::uniformVec3(const std::string location, const vec3 &vec) {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform3f(transformLoc, vec.x, vec.y, vec.z);
}

void Shader::uniformInt(const std::string location, int value) {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}

void Shader::uniformFloat(const std::string location, float value) {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1f(transformLoc,value);
}

void Shader::uniformBool(const std::string location, bool value) {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}
