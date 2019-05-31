#include "rendering/Shader.hpp"

#include <glad/glad.h>

#include <string>
#include <regex>

#include <fstream>
#include <sstream>

static std::string readShader(std::ifstream &file, std::string directory) {
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

std::string str_replace( std::string const & in, std::string const & from, std::string const & to )
{
    return std::regex_replace( in, std::regex(from), to );
}


Shader::Shader() {

}

void Shader::load(std::string vertex, std::string fragment, std::string geometry) {
    const char* vShaderCode = vertex.c_str();
    const char* fShaderCode = fragment.c_str();
    const char* fGeometryCode = geometry.c_str();

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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl << vertex << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl << fragment << std::endl;
    }

    unsigned int geometryShader = 0;

    if (!geometry.empty()) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &fGeometryCode, NULL);
        glCompileShader(geometryShader);
    }

    this->id = glCreateProgram();

    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);
    if (!geometry.empty()) {
        glAttachShader(this->id, geometryShader);
    }

    glLinkProgram(this->id);

    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(this->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::loadFile(const Path &file) {
    Path directory = file.getParent();

    std::ifstream stream(file);

    if (stream.is_open()) {
        std::string code = readShader(stream, directory);

        
        unsigned first = code.find("#pragma BEGINVERTEX");
        unsigned last = code.find("#pragma ENDVERTEX");
        std::string vertexCode = code.substr (first,last-first);
        vertexCode = "#version 330 core\n" + vertexCode;

        first = code.find("#pragma BEGINFRAGMENT");
        last = code.find("#pragma ENDFRAGMENT");
        std::string fragmentCode = code.substr (first,last-first);
        fragmentCode = "#version 330 core\n" + fragmentCode;

        stream.close();

        this->load(vertexCode, fragmentCode);
    }
    else {
        std::cerr << "error loading shader: " << file << std::endl;
    }
}


void Shader::bind() const {
    glUseProgram(this->id);
}

void Shader::uniformMat4(const std::string &location, const mat4 &mat) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());

    float matrixArray[] = {
        mat.m00, mat.m10, mat.m20, mat.m30,
        mat.m01, mat.m11, mat.m21, mat.m31,
        mat.m02, mat.m12, mat.m22, mat.m32,
        mat.m03, mat.m13, mat.m23, mat.m33
    };
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matrixArray);
}

void Shader::uniformVec3(const std::string &location, const vec3 &vec) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform3f(transformLoc, vec.x, vec.y, vec.z);
}

void Shader::uniformVec4(const std::string &location, const vec4 &vec) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform4f(transformLoc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uniformInt(const std::string &location, int value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}

void Shader::uniformFloat(const std::string &location, float value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1f(transformLoc,value);
}

void Shader::uniformBool(const std::string &location, bool value) const {
    unsigned int transformLoc = glGetUniformLocation(this->id, location.c_str());
    glUniform1i(transformLoc,value);
}
