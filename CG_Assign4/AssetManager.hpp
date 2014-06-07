#pragma once
#include "GLHeaders.hpp"
#include <map>
#include <string>

class AssetManager {
public:
    static GLint loadTexture(const std::string& filename);

private:
    static std::map<std::string, GLint> textures;
};