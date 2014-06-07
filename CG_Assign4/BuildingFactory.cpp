#include "BuildingFactory.hpp"
#include <ctime>

#define rand_pos_neg ((rand() % 2) * 2 - 1)

float randFloat(float min, float max)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

BuildingFactory::BuildingFactory(): buildingDimension(1.0), buildingHeight(5.0) {}

RawModelData genTrianglePrism(const std::string& texture, float width, float height, float depth, glm::vec3 center) {
    RawModelData data;
    RawModelData::Shape shape;

    // Bottom
    shape = shapes::quad(glm::vec3(width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(-width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, depth + center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Left
    shape = shapes::triangle(glm::vec3(-width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Right
    shape = shapes::triangle(glm::vec3(width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Front
    shape = shapes::triangle(glm::vec3(width + center.x, -height + center.y, -depth + center.z), glm::vec3(-width + center.x, -height + center.y, -depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    // Back
    shape = shapes::triangle(glm::vec3(-width + center.x, -height + center.y, depth + center.z), glm::vec3(width + center.x, -height + center.y, depth + center.z),
        glm::vec3(center.x, height + center.y, center.z));
    shape.textureName = texture;
    data.shapes.push_back(shape);

    return data;
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
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, -1, depth + center.z),
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

    return data;
}

RawModelData BuildingFactory::genBlockBuilding(const std::string& texture) {    
    const float firstBlockSize = randFloat(0.5, 1.0);

    // Original square
    RawModelData data;
    data = genCube(texture, buildingDimension, 0.1f, buildingDimension, glm::vec3(0, 0, 0));
    
    // First block
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

    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-buildingDimension, -1, -buildingDimension);
    boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight, buildingDimension);
    data.boundingBox = boundingBox;

    return data;
}

RawModelData BuildingFactory::genClassicBuilding(const std::string& texture) {    
    const float triangleHeight = randFloat(0.0, 0.3);

    const float startingHeight = (buildingHeight - triangleHeight) * 0.6;
    const float secondHeight = (buildingHeight - triangleHeight) * 0.8;
    const float thirdHeight = (buildingHeight - triangleHeight);

    // Base structure
    RawModelData data; 
    data = genCube(texture, buildingDimension, startingHeight, buildingDimension, glm::vec3(0, 0, 0));

    // Second
    RawModelData block;
    block = genCube(texture, buildingDimension * 0.8, secondHeight, buildingDimension * 0.8, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third
    block = genCube(texture, buildingDimension * 0.6, thirdHeight, buildingDimension * 0.6, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Triangle top
    if (triangleHeight > 0.1) {
        block = genTrianglePrism(texture, buildingDimension * 0.6, triangleHeight, buildingDimension * 0.6, glm::vec3(0, buildingHeight, 0));
        data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());
    }

    return data;

}

std::vector <RawModelData> BuildingFactory::genBuildings(const std::string& texture, int number)
{   
    std::vector <RawModelData> buildings;
    for (int i=0; i<number; i++) {
        if (i % 2) buildings.push_back(genClassicBuilding(texture));
        else buildings.push_back(genBlockBuilding(texture));
    }
    return buildings;
}
