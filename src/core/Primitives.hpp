#pragma once

#include "Mesh.hpp"

class Primitives {
public:
    static Mesh torus(float r1, float r2, int numSteps1, int numSteps2);

	static Mesh sphere(unsigned int xSegments, unsigned int ySegments);

    static Mesh sprite();

	static Mesh framebuffer();

    static Mesh cube(float scale=1.0f, float width=1.0f, float height=1.0f, float depth=1.0f);

    static Mesh skybox();
};