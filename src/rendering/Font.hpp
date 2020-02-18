#pragma once

#include <string>
#include "rendering/Texture.hpp"

#include <map>

struct Character {
    Texture tex;
    vec2i origin;       // Size of glyph
    vec2i size;    // Offset from baseline to left/top of glyph
    int advance;    // Offset to advance to next glyph
};

class Font {
private:
    

public:
    int ascent;
    int descent;
    int lineGap;

    float scale;

    int fontSize;

    std::map<unsigned char, Character> characters;



    Font();

    ~Font();

    void loadFromFile(const std::string &path, unsigned int fontSize);

    vec2 getTextSize(const std::string &text) const;
};