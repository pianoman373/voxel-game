#include "client/BlockTextureAtlas.hpp"


#include <stb_image.h>

int BlockTextureAtlas::registerTexture(const std::string &path) {
    if (ids.find(path) == ids.end()) {
        filePaths.push_back(path);
        ids[path] = filePaths.size();
    }

    return ids[path];
}

void BlockTextureAtlas::buildAtlas() {
    std::vector<unsigned char> data;

    int textureSize = 32;

    for (int x = 0; x < textureSize; x++) {
        for (int y = 0; y < textureSize; y++) {

            if (x < textureSize/2 && y < textureSize/2) {
                data.push_back(255);
                data.push_back(0);
                data.push_back(255);
            }
            else if (x >= textureSize/2 && y >= textureSize/2) {
                data.push_back(255);
                data.push_back(0);
                data.push_back(255);
            }
            else {
                data.push_back(0);
                data.push_back(0);
                data.push_back(0);
            }
            
            data.push_back(255);
        }
    }

    int width, height, components;

    for (int i = 0; i < filePaths.size(); i++) {
        unsigned char* image = stbi_load(filePaths[i].c_str(), &width, &height, &components, STBI_rgb_alpha);
        int size = width*height*components;
        
        for (int j = 0; j < size; j++) {
            data.push_back(image[j]);
        }

        if (image) {
            stbi_image_free(image);
        }
        else {
            std::cerr << "error loading texture: " << filePaths[i] << std::endl;
        }
    }

    atlas.load(data.data(), width, height, filePaths.size()+1);
}

TextureArray BlockTextureAtlas::getTexture() {
    return atlas;
}