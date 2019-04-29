#pragma once

#include "Mesh.hpp"
#include "Material.hpp"
#include "Path.hpp"

#include <vector>
#include <string>

struct ModelNode {
    Mesh *mesh;
    int materialIndex;
    std::string name;
};

class Model {
public:
    std::vector<Material> materials;
    std::vector<ModelNode> nodes;

    void addSubmesh(const Mesh &mesh, const Material &material, const std::string &names="Untitled Submesh");

    void openFile(const Path &filename);

    void fromJson(const json &j, const Path &workingDirectory);

    json toJson(const Path &workingDirectory) const;

    void clear();
};