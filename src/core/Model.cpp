#include "Model.hpp"
#include "Renderer.hpp"
#include "Resources.hpp"

#include <string>
#include <algorithm>
#include <fstream>

void Model::addSubmesh(const Mesh &mesh, const Material &material, const std::string &name) {
    // materials.push_back(material);

    // ModelNode node;
    // node.mesh = mesh;
    // node.materialIndex = materials.size();
    // node.name = name;

    // nodes.push_back(node);
}

void Model::openFile(const Path &filename) {
    Path workingDirectory = filename.getParent();

    json j;
    std::ifstream o(filename);
    o >> j;


    fromJson(j, workingDirectory);
}

void Model::fromJson(const json &j, const Path &workingDirectory) {

    json jMaterials = j["materials"];

    for (size_t i = 0; i < jMaterials.size(); i++) {
        json jMaterial = jMaterials[i];

        Material mat;

        mat.setShader(Resources::standardShader);
        mat.setDefaultPBRUniforms();
        mat.fromJson(jMaterial, workingDirectory);

        materials.push_back(mat);
    }


    json jMeshes = j["meshes"];

    for (size_t i = 0; i < jMeshes.size(); i++) {
        json jMesh = jMeshes[i];


        // ModelNode node;
        // node.mesh.fromJson(jMesh["data"]);
        // node.mesh.generate();

        // node.materialIndex = jMesh["materialIndex"];
        // node.name = jMesh["name"];

        // nodes.push_back(node);
    }
}

json Model::toJson(const Path &workingDirectory) const {
   json j;

    for (size_t i = 0; i < nodes.size(); i++) {
        ModelNode node = nodes[i];
        json jMesh;
        jMesh["name"] = node.name;
        jMesh["data"] = node.mesh->toJson();
        jMesh["materialIndex"] = node.materialIndex;

        j["meshes"][i] = jMesh;
    }

    for (size_t i = 0; i < materials.size(); i++) {
        Material mat = materials[i];

        j["materials"][i]  = mat.toJson(workingDirectory);
    }

    return j;
}

void Model::clear() {
    nodes.clear();
    materials.clear();
}
