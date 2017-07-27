#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glad/glad.h>

void Texture::load(const char *file) {
    stbi_set_flip_vertically_on_load(false);
    int width, height;
    unsigned char* image = stbi_load(file, &width, &height, 0, STBI_rgb_alpha);

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    //glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadFromSingleColor(vec4 color) {
    unsigned char image[4] = {(unsigned char)(color.x * 255.0f), (unsigned char)(color.y * 255.0f), (unsigned char)(color.z * 255.0f), (unsigned char)(color.w * 255.0f)};

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int Texture::getID() {
    return id;
}

void Cubemap::load(const char *file1, const char *file2, const char *file3, const char *file4, const char *file5, const char *file6) {
    stbi_set_flip_vertically_on_load(false);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 10);

    //stbi_set_flip_vertically_on_load(false);

    const char *files[] = {file1, file2, file3, file4, file5, file6};

    //iterate over all 6 textures and send their raw data to the cubemap
    for (int i = 0; i < 6; i++) {
        int width, height;
        float* image = stbi_loadf(files[i], &width, &height, 0, STBI_rgb);



        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image);
    }

    //interpolation settings and texture wrapping for the texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Cubemap::bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

unsigned int Cubemap::getID() {
    return id;
}
