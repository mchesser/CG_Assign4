//! A class to create random rawModelData buildings
#pragma once

#include <vector>
#include "Shapes.hpp"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

class BuildingFactory {
public:
    BuildingFactory(std::vector <std::string> windowTexturesName, std::string topTextureName);

    /// <summary>
    /// Returns a block styled building
    /// </summary>
    RawModelData genBlockBuilding();

    /// <summary>
    /// Returns a classic styled building
    /// </summary>
    RawModelData genClassicBuilding();

    /// <summary>
    /// Returns a number of random generated buildings
    /// </summary>
    std::vector <RawModelData> genBuildings(int number);
private:
    float buildingDimension;
    float buildingHeight;
    std::vector <std::string> windowTextures;
    std::string topTexture;

    RawModelData genTrianglePrism(float width, float height, float depth, glm::vec3 center);
    RawModelData genCube(std::string texture, float width, float height, float depth, glm::vec3 center);

};