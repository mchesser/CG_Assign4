#include "BuildingFactory.hpp"
#include <ctime>

#define rand_pos_neg ((rand() % 2) * 2 - 1)

float randFloat(float min, float max) {
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

BuildingFactory::BuildingFactory(std::vector <std::string> windowTexturesName, std::string topTextureName) 
    : buildingDimension(1.0), buildingHeight(5.0), windowTextures(windowTexturesName), topTexture(topTextureName) {
}

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

RawModelData BuildingFactory::genCube(std::string texture, float width, float height, float depth, glm::vec3 center) {
    RawModelData data;
    RawModelData::Shape shape;

    // Top
    shape = shapes::quad(glm::vec3(-width + center.x, height, depth + center.z), glm::vec3(-width + center.x, height, -depth + center.z),
        glm::vec3(width + center.x, height, -depth + center.z), glm::vec3(width + center.x, height, depth + center.z));
    shape.textureName = topTexture;
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

RawModelData BuildingFactory::genBlockBuilding() {
    const float firstBlockSize = randFloat(0.5f, 1.0f);
    const float zFightOffset = 0.05f;

    // Get random building side texture
    const std::string sideTexture = windowTextures[rand() % windowTextures.size()];

    // Original square
    RawModelData data;
    data = genCube(sideTexture, buildingDimension, 0.1f, buildingDimension, glm::vec3(0, 0, 0));

    // First block
    RawModelData block;
    block = genCube(sideTexture,
        firstBlockSize - zFightOffset,
        buildingHeight,
        firstBlockSize - zFightOffset,
        glm::vec3(rand_pos_neg * (buildingDimension - firstBlockSize), 0, rand_pos_neg * (buildingDimension - firstBlockSize)));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Second block
    block = genCube(sideTexture,
        randFloat(0.3f, 0.8f - zFightOffset),
        buildingHeight * randFloat(0.3f, 1.0f - zFightOffset),
        buildingDimension - 2 * zFightOffset,
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third block
    block = genCube(sideTexture,
        buildingDimension - 2 * zFightOffset,
        buildingHeight * randFloat(0.8f, 1.0f - zFightOffset),
        randFloat(0.3f, 0.8f - zFightOffset),
        glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-buildingDimension, -1, -buildingDimension);
    boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight, buildingDimension);
    data.boundingBox = boundingBox;

    return data;
}

RawModelData BuildingFactory::genClassicBuilding() {
    const float triangleHeight = randFloat(0.0f, 0.3f);

    const float startingHeight = (buildingHeight - triangleHeight) * 0.6f;
    const float secondHeight = (buildingHeight - triangleHeight) * 0.8f;
    const float thirdHeight = (buildingHeight - triangleHeight);

    // Get random building side texture
    const std::string sideTexture = windowTextures[rand() % windowTextures.size()];

    // Base structure
    RawModelData data;
    data = genCube(sideTexture, buildingDimension, startingHeight, buildingDimension, glm::vec3(0, 0, 0));

    // Second
    RawModelData block;
    block = genCube(sideTexture, buildingDimension * 0.8f, secondHeight, buildingDimension * 0.8f, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    // Third
    block = genCube(sideTexture, buildingDimension * 0.6f, thirdHeight, buildingDimension * 0.6f, glm::vec3(0, 0, 0));
    data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());

    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-buildingDimension, -1, -buildingDimension);
    boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight, buildingDimension);
    data.boundingBox = boundingBox;

    // Triangle top
    if (triangleHeight > 0.1) {
        block = genTrianglePrism(buildingDimension * 0.6f, triangleHeight, buildingDimension * 0.6f, glm::vec3(0, buildingHeight, 0));
        data.shapes.insert(data.shapes.end(), block.shapes.begin(), block.shapes.end());
        boundingBox.maxVertex = glm::vec3(buildingDimension, buildingHeight + triangleHeight, buildingDimension);
    }

    return data;

}

std::vector <RawModelData> BuildingFactory::genBuildings(int number) {
    std::vector <RawModelData> buildings;
    for (int i = 0; i < number; i++) {
        if (i % 2) buildings.push_back(genClassicBuilding());
        else buildings.push_back(genBlockBuilding());
    }
    return buildings;
}
