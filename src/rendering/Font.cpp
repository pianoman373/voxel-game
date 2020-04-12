#include "rendering/Font.hpp"


#include <ft2build.h>
#include FT_FREETYPE_H


#include <fstream>
#include <glad/glad.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

unsigned char buffer[24<<20];

Font::Font() {
}

Font::~Font() {
}

void Font::loadFromFile(const std::string &path, unsigned int fontSize) {
    stbtt_fontinfo font;

    this->fontSize = fontSize;

    fread(buffer, 1, 1000000, fopen(path.c_str(), "rb"));
    stbtt_InitFont(&font, buffer, 0);

    
    scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);


    for (unsigned char c = 0; c < 255; c++)
    {
        int width, height, xoff, yoff, advanceWidth, leftSideBearing;
        unsigned char* buffer = stbtt_GetCodepointBitmapSubpixel(&font, scale, scale, 0.0f, 0.0f, c, &width, &height, &xoff, &yoff);


        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBox(&font, c, &c_x1, &c_y1, &c_x2, &c_y2);

        Texture tex;
        tex.load(buffer, width, height, true, true);


        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);

         // Now store character for later use
        Character character = {
                tex,
                vec2i(c_x1*scale, c_y1*scale - descent*scale),
                vec2i((c_x2-c_x1)*scale, (c_y2-c_y1)*scale),
                advanceWidth
        };
        this->characters.insert(std::pair<char, Character>(c, character));
    }
}

vec2 Font::getTextSize(const std::string &text) const {
    vec2 ret = vec2(0.0f, fontSize);

   for (size_t i = 0; i < text.size(); i++) {
        const Character &ch = characters.at(text[i]);

       ret.x += (ch.advance*scale);

    }

    return ret;
}