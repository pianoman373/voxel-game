#pragma once

#include "Mesh.hpp"

class Primitives {
public:
    static void torus(Mesh &m, float r1, float r2, int numSteps1, int numSteps2);

    static void cube(Mesh &m);
};