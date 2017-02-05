#pragma once

class Texture {
private:
    unsigned int id;
public:
    Texture(const char *file);

    void bind();
};
