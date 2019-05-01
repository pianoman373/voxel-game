#pragma once

#include <string>
#include "rendering/Texture.hpp"

#include <map>

struct Character {
    Texture tex;
    vec2i size;       // Size of glyph
    vec2i bearing;    // Offset from baseline to left/top of glyph
    int advance;    // Offset to advance to next glyph
};

class Font {
private:
    unsigned int size;

public:
    int descender;

    std::map<unsigned char, Character> characters;



    Font();

    ~Font();

    void loadFromFile(const std::string &path, unsigned int fontSize);

    vec2 getTextSize(const std::string &text) const;
};