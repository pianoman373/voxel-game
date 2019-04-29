#pragma once

#include <vector>
#include <string>

#include "Math.hpp"
#include "IRenderable.hpp"

#include "json.hpp"
using nlohmann::json;


class Mesh : public IRenderable {
private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    int length = 0;

public:
    std::vector<vec3> positions;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<unsigned int> indices;

    std::vector<vec4i> boneIDs;
    std::vector<vec4> boneWeights;

    int renderMode = 0x0004;

    Mesh();

    Mesh(const std::vector<vec3> &positions, const std::vector<unsigned int> &indices);

    Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<unsigned int> &indices);

    Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<vec2> &uvs, const std::vector<unsigned int> &indices);

    Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<vec2> &uvs, const std::vector<vec3> &colors, const std::vector<unsigned int> &indices);

    json toJson() const;

    void fromJson(const json &j);

    /**
     * Sends buffered data to OpenGL to make this mesh ready for rendering. (Note this does not clear buffered data as OpenGL still needs it)
     */
    void generate();

    /**
     * Clears local buffer data.
     */
    void clear();

    void render() const;

    /**
     *  Deletes mesh handles on OpenGL and local buffer data.
     */
    void destroy();
};
