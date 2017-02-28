#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <GL/glew.h>

Texture::Texture(const char *file) {
    int width, height;
    unsigned char* image = stbi_load(file, &width, &height, 0, STBI_rgb);

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, id);
}
