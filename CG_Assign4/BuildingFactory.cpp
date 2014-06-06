#include "BuildingFactory.hpp"
#include <iostream>

#define rand_pos_neg ((rand() % 2) * 2 - 1)

float randFloat(float min, float max)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

RawModelData genCube(const std::string& texture, float width, float height, float depth, glm::vec3 center) {
    RawModelData data;
    RawModelData::Shape shape;

    // Top
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, height, -depth + center.z),
        glm::vec3(width + center.x, height, -depth + center.z), glm::vec3(width + center.x, height, depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Bottom
    shape = shapes::quad(glm::vec3(width + center.x, -1, depth + center.z), glm::vec3(width + center.x, -1, -depth + center.z),
        glm::vec3(-width + center.x, -1, -depth + center.z), glm::vec3(-width + center.x, -1, depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Left
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, -height, depth + center.z),
        glm::vec3(-width + center.x, -1, -depth + center.z), glm::vec3(-width + center.x, height, -depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Right
    shape = shapes::quad(glm::vec3(width + center.x, height, -depth + center.z), glm::vec3(width + center.x, -1, -depth + center.z),
        glm::vec3(width + center.x, -1, depth + center.z), glm::vec3(width + center.x, height, depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Front
    shape = shapes::quad(glm::vec3(-width + center.x, height, -depth + center.z), glm::vec3(-width + center.x, -1, -depth + center.z),
        glm::vec3(width + center.x, -1, -depth + center.z), glm::vec3(width + center.x, height, -depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Back
    shape = shapes::quad(glm::vec3(width + center.x, height, depth + center.z), glm::vec3(width + center.x, -1, depth + center.z),
        glm::vec3(-width + center.x, -1, depth + center.z), glm::vec3(-width + center.x, height, depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // BoundingBox of whole cube 
    //  - May need to put this in genBuilding if its to slow
    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-width + center.x, -1, -depth + center.z);
    boundingBox.maxVertex = glm::vec3(width + center.z, height, depth + center.z);
    data.boundingBox = boundingBox;

    return data;
}

RawModelData BuildingFactory::genBlockBuilding(const std::string& texture) {
    srand(time(NULL));
    
    const float buildingDimension = 1.0;
    const float buildingHeight = 5.0;

    // Original square
    RawModelData data;
    data = genCube(texture, buildingDimension, 0.1f, buildingDimension, glm::vec3(0, 0, 0));

    // First block
    const float firstBlockSize = 0.7;
    RawModelData block;
    block = genCube(texture,
        firstBlockSize,
        buildingHeight,
        firstBlockSize,
        glm::vec3(rand_pos_neg * (buildingDimension - firstBlockSize), 0, rand_pos_neg * (buildingDimension - firstBlockSize)));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Second block
    block = genCube(texture,
        randFloat(0.3, 0.8),
        buildingHeight * randFloat(0.3, 1.0),
        buildingDimension,
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third block
    block = genCube(texture,
        buildingDimension,
        buildingHeight * randFloat(0.8, 1.0),
        randFloat(0.3, 0.8),
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    return data;
}

std::vector <RawModelData> BuildingFactory::genBuildings(const std::string& texture, int number)
{
    std::vector <RawModelData> buildings;
    for (int i=0; i<number; i++) {
        buildings.push_back(genBlockBuilding(texture));
    }
    return buildings;
}
