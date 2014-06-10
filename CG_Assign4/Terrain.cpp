#include "Terrain.hpp"
#include <iostream>

#define HORIZONTAL_TEXTURE "data/ground/RoadstraightHorizontal.jpg"
#define VERTICAL_TEXTURE "data/ground/RoadstraightVertical.jpg"
#define INTERSECTION_TEXTURE "data/ground/Intersection.jpg"
#define BUILDING_GROUND_TEXTURE "data/ground/Vereda.jpg"

#define HORIZONTAL_NORMAL_TEXTURE "data/ground/RoadstraightHorizontal_NORMAL.png"
#define VERTICAL_NORMAL_TEXTURE "data/ground/RoadstraightVertical_NORMAL.png"
#define INTERSECTION_NORMAL_TEXTURE "data/ground/RoadstraightIntersection_NORMAL.png"
#define BUILDING_GROUND_NORMAL_TEXTURE "data/ground/Vereda_NORMAL.png"

// Generates a square tile with the specified texture loaded from a file
RawModelData genTerrainModel(const std::string& terrainTexture, const std::string& normalTexture) {

    RawModelData data;  
    RawModelData::Shape shape = shapes::quad(glm::vec3(-1, 0, 1), glm::vec3(-1, 0, -1),
        glm::vec3(1, 0, -1), glm::vec3(1, 0, 1));
    shape.textureName = terrainTexture;
    shape.normalMap = normalTexture;
    data.shapes.push_back(shape);

    // BoundingBox of terrain
    BoundingBox boundingBox;
    boundingBox.minVertex = glm::vec3(-1, 0, -1);
    boundingBox.maxVertex = glm::vec3(1, 0, 1);
    data.boundingBox = boundingBox;

    return data;
}

Terrain::Terrain(Renderer* renderer) {
    horizontalRoad = new ModelData(genTerrainModel(HORIZONTAL_TEXTURE, HORIZONTAL_NORMAL_TEXTURE), renderer);
    verticalRoad = new ModelData(genTerrainModel(VERTICAL_TEXTURE, VERTICAL_NORMAL_TEXTURE), renderer);
    intersection = new ModelData(genTerrainModel(INTERSECTION_TEXTURE, INTERSECTION_NORMAL_TEXTURE), renderer);
    building = new ModelData(genTerrainModel(BUILDING_GROUND_TEXTURE, BUILDING_GROUND_NORMAL_TEXTURE), renderer);
}

void Terrain::draw(Renderer* renderer, City* city, glm::vec3 cameraPosition, int size) const {

    const float terrainSizeX = 1.2;
    const float terrainSizeZ = terrainSizeX;

    // Find camera position square
    glm::vec3 centerSquare = glm::vec3( 
        terrainSizeX * 2 * (int)((cameraPosition.x + terrainSizeX * cameraPosition.x/(fabs(cameraPosition.x)))/(terrainSizeX * 2)),
        0.0, 
        terrainSizeZ * 2 * (int)((cameraPosition.z + terrainSizeZ * cameraPosition.z/(fabs(cameraPosition.z)))/(terrainSizeZ * 2))
        );

    // Handle division by 0
    if (cameraPosition.x == 0) centerSquare.x = 0.0;
    if (cameraPosition.z == 0) centerSquare.z = 0.0;

    // Draw center square and surrounding squares
    for (int i=-size; i<size+1; i++) { 
        for (int j=-size; j<size+1; j++) {
            glm::vec3 square = centerSquare;
            square.x = centerSquare.x + terrainSizeX * 2 * j;
            square.z = centerSquare.z + terrainSizeZ * 2 * i;
            switch (city->tileForPosition(square)) {
            case B: // Building case
            {
            	renderer->drawModel(building, square, glm::vec3(terrainSizeX, 1, terrainSizeZ));
            }
                break;
            case V: // Vertical road segment
            { 
            	renderer->drawModel(verticalRoad, square, glm::vec3(terrainSizeX, 1, terrainSizeZ));
            }
                break;
            case H: // Horizontal road segment
            {
            	renderer->drawModel(horizontalRoad, square, glm::vec3(terrainSizeX, 1, terrainSizeZ));
            }
                break;
            case I: // Intersection
            {
            	renderer->drawModel(intersection, square, glm::vec3(terrainSizeX, 1, terrainSizeZ));
            }
                break;
            }
        }
    }
}
