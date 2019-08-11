#include "client/EntityRenderer.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Primitives.hpp"
#include "client/WorldRenderer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

EntityRenderer::EntityRenderer(WorldRenderer &wr): wr(wr) {
    tex.load("mods/base/resources/skeleton.png");

    material.setShader(wr.entityShader);
    material.setUniformTexture("tex", tex);

    std::string inputfile = "mods/base/resources/skeleton.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) {
    std::cerr << err << std::endl;
    }

    if (!ret) {
    exit(1);
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;

    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = shapes[s].mesh.num_face_vertices[f];

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
        tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
        tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
        tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
        tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
        tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
        tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
        tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

        mesh.positions.push_back(vec3(vx, vy, vz));
        mesh.normals.push_back(vec3(nx, ny, nz));
        mesh.uvs.push_back(vec2(tx, ty));


        // Optional: vertex colors
        // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
        // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
        // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
        }
        index_offset += fv;

        // per-face material
        shapes[s].mesh.material_ids[f];
    }
    }

    mesh.generate();
}

void EntityRenderer::render(Entity &e) {
    //Renderer::debug.renderDebugAABB(e.getPosition() - (vec3(e.width, e.height, e.depth) / 2.0f), e.getPosition() + (vec3(e.width, e.height, e.depth) / 2.0f), vec3(1.0f, 0.0f, 0.0f));
    
    

    AABB aabb;
    aabb.min = e.getPosition() - (vec3(e.width, e.height, e.depth) / 2.0f);
    aabb.max = e.getPosition() + (vec3(e.width, e.height, e.depth) / 2.0f);

    Transform transform;
    transform.position = e.getPosition() - vec3(0.0f, e.height / 2.0f, 0.0f);
    transform.scale = vec3(0.32f);
    transform.rotation = rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), normalize(vec3(e.xDirection, 0.0f, e.zDirection)));

    

    Renderer::debug.renderDebugLine(e.getPosition(), e.getPosition() + normalize(vec3(e.xDirection, 0.0f, e.zDirection)), vec3(0.0f, 1.0f, 0.0f));
    

    Renderer::render(&mesh, &material, transform.getMatrix(), aabb);
}