#include "AssetManager.hpp"
#include "SOIL2/SOIL2.h"

std::map<std::string, GLint> AssetManager::textures;

GLint AssetManager::loadTexture(const std::string& filename) {
    if (AssetManager::textures.find(filename) == AssetManager::textures.end()) {
        // Texture has not been loaded already
        GLint textureId = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, 
            SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
        AssetManager::textures[filename] = textureId;
        return textureId;
    }
    return AssetManager::textures[filename];
}