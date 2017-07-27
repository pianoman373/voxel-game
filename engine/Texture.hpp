#pragma once

#include "Math.hpp"

class Texture {
private:
    unsigned int id;
public:
    void load(const char *file);

    void loadFromSingleColor(vec4 color);

    void bind(unsigned int unit = 0);

    unsigned int getID();
};

class Cubemap {
private:
    unsigned int id;

public:
    void load(const char *file1, const char *file2, const char *file3, const char *file4, const char *file5, const char *file6);

    void bind(unsigned int unit = 0);

    unsigned int getID();
};
