//! Class for managing static resources
#pragma once
#include "GLHeaders.hpp"
#include <map>
#include <string>

class AssetManager {
public:
    /// <summary>
    /// Load a texture returning its OpenGL id
    /// </summary>
    ///
    /// <param name="filename">The name of the texture file</param>
    static GLint loadTexture(const std::string& filename);

private:
    static std::map<std::string, GLint> textures;
};