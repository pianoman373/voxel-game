#include "rendering/Font.hpp"


#include <ft2build.h>
#include FT_FREETYPE_H


#include <fstream>
#include <glad/glad.h>

Font::Font() {
}

Font::~Font() {
}

void Font::loadFromFile(const std::string &path, unsigned int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    for (unsigned char c = 0; c < 255; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        Texture tex;
        tex.load(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, false, true);

        // Now store character for later use
        Character character = {
                tex,
                vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<int>(face->glyph->advance.x)
        };
        this->characters.insert(std::pair<char, Character>(c, character));
    }

    this->size = (face->size->metrics.ascender >> 6) - (face->size->metrics.descender >> 6);
    this->descender = -(face->size->metrics.descender >> 6);


    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

vec2 Font::getTextSize(const std::string &text) const {
    vec2 ret = vec2(0.0f, static_cast<float>(size));

   for (size_t i = 0; i < text.size(); i++) {
        const Character &ch = characters.at(text[i]);

       ret.x += (ch.advance >> 6);

    }

    return ret;
}