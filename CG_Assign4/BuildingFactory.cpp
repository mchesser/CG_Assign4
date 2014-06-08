#include "BuildingFactory.hpp"
#include <ctime>

#define rand_pos_neg ((rand() % 2) * 2 - 1)

float randFloat(float min, float max)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

BuildingFactory::BuildingFactory(std::vector <std::string> windowTexturesName, std::string topTextureName): 
    buildingDimension(1.0),
    buildingHeight(5.0),
    windowTextures(windowTexturesName),
    topTexture(topTextureName)
{}

RawModelData BuildingFactory::genTrianglePrism(float width, float height, float depth, glm::vec3 center) {
    RawModelData data;
    RawModelData::Shape shape;

    // Bottom
    shape = shapes::quad(glm::vec3(width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(-width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, depth + center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    // Left
    shape = shapes::triangle(glm::vec3(-width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    // Right
    shape = shapes::triangle(glm::vec3(width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    // Front
    shape = shapes::triangle(glm::vec3(width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    // Back
    shape = shapes::triangle(glm::vec3(-width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    return data;
}

RawModelData BuildingFactory::genCube(float width, float height, float depth, glm::vec3 center) {
    RawModelData data;
    RawModelData::Shape shape;
   
    // Get random building side texture
    const std::string sideTexture = windowTextures[rand() % windowTextures.size()]; 

    // Top
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, height, -depth + center.z),
        glm::vec3(width + center.x, height, -depth + center.z), glm::vec3(width + center.x, height, depth + center.z));
    shape.textureName = topTexture;
    data.shapes.push_back(shape);

    // Bottom
    shape = shapes::quad(glm::vec3(width + center.x, -1, depth + center.z), glm::vec3(width + center.x, -1, -depth + center.z),
        glm::vec3(-width + center.x, -1, -depth + center.z), glm::vec3(-width + center.x, -1, depth + center.z));
    shape.textureName = sideTexture;
    data.shapes.push_back(shape);
    
    // Left
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, -1, depth + center.z),
        glm::vec3(-width + center.x, -1, -depth + center.z), glm::vec3(-width + center.x, height, -depth + center.z));
    shape.textureName = sideTexture;
    data.shapes.push_back(shape);

    // Right
    shape = shapes::quad(glm::vec3(width + center.x, height, -depth + center.z), glm::vec3(width + center.x, -1, -depth + center.z),
        glm::vec3(width + center.x, -1, depth + center.z), glm::vec3(width + center.x, height, depth + center.z));
    shape.textureName = sideTexture;
    data.shapes.push_back(shape);

    // Front
    shape = shapes::quad(glm::vec3(-width + center.x, height, -depth + center.z), glm::vec3(-width + center.x, -1, -depth + center.z),
        glm::vec3(width + center.x, -1, -depth + center.z), glm::vec3(width + center.x, height, -depth + center.z));
    shape.textureName = sideTexture;
    data.shapes.push_back(shape);

    // Back
    shape = shapes::quad(glm::vec3(width + center.x, height, depth + center.z), glm::vec3(width + center.x, -1, depth + center.z),
        glm::vec3(-width + center.x, -1, depth + center.z), glm::vec3(-width + center.x, height, depth + center.z));
    shape.textureName = sideTexture;
    data.shapes.push_back(shape);

    return data;
}

RawModelData BuildingFactory::genBlockBuilding() {    
    const float firstBlockSize = randFloat(0.5, 1.0);

    // Original square
    RawModelData data;
    data = genCube(buildingDimension, 0.1f, buildingDimension, glm::vec3(0, 0, 0));
    
    // First block
    RawModelData block;
    block = genCube(
        firstBlockSize,
        buildingHeight,
        firstBlockSize,
        glm::vec3(rand_pos_neg * (buildingDimension - firstBlockSize), 0, rand_pos_neg * (buildingDimension - firstBlockSize)));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Second block
    block = genCube(
        randFloat(0.3, 0.8),
        buildingHeight * randFloat(0.3, 1.0),
        buildingDimension,
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third block
    block = genCube(
        buildingDimension,
        buildingHeight * randFloat(0.8, 1.0),
        randFloat(0.3, 0.8),
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-buildingDimension, -1, -buildingDimension);
    boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight, buildingDimension);
    data.boundingBox = boundingBox;

    return data;
}

RawModelData BuildingFactory::genClassicBuilding() {    
    const float triangleHeight = randFloat(0.0, 0.3);

    const float startingHeight = (buildingHeight - triangleHeight) * 0.6;
    const float secondHeight = (buildingHeight - triangleHeight) * 0.8;
    const float thirdHeight = (buildingHeight - triangleHeight);

    // Base structure
    RawModelData data; 
    data = genCube(buildingDimension, startingHeight, buildingDimension, glm::vec3(0, 0, 0));

    // Second
    RawModelData block;
    block = genCube(buildingDimension * 0.8, secondHeight, buildingDimension * 0.8, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third
    block = genCube(buildingDimension * 0.6, thirdHeight, buildingDimension * 0.6, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-buildingDimension, -1, -buildingDimension);
    boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight, buildingDimension);
    data.boundingBox = boundingBox;

    // Triangle top
    if (triangleHeight > 0.1) {
        block = genTrianglePrism(buildingDimension * 0.6, triangleHeight, buildingDimension * 0.6, glm::vec3(0, buildingHeight, 0));
        data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());
        boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight + triangleHeight, buildingDimension);
    }

    return data;

}

std::vector <RawModelData> BuildingFactory::genBuildings(int number)
{   
    std::vector <RawModelData> buildings;
    for (int i=0; i<number; i++) {
        if (i % 2) buildings.push_back(genClassicBuilding());
        else buildings.push_back(genBlockBuilding());
    }
    return buildings;
}
