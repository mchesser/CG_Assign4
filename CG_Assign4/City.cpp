#include "City.hpp"
#include <stdlib.h>

// FIXME: This should be dynamic
#define BULDING_SCALE (2.0f)
// FIXME: This should by dynamic
#define BULDING_OFFSET (2.0f)

// FIXME: if we want buildings to have a unique street facing facade this value needs to be
// dynamic
#define STREET_DIR (glm::vec3(0, 0, 1))
#define SKY_DIR (glm::vec3(0, 1, 0))

// FIXME: Very poor way of generating a bounded random int
#define rand_max(max) (rand() % max)
// FIXME: Very poor way of generating a random float
#define randf() (static_cast<float>(rand()) / RAND_MAX)

City::City(const ModelData* base_model) : data(20, 20, 0), center(glm::vec2(0, 0)) {

    // Generate 10 buildings of different heights
    // FIXME: There might be other ways of varying the buildings
    building_types.reserve(10);
    for (size_t i = 0; i < 10; ++i) {
        BuildingData building = {
            glm::vec3(BULDING_SCALE / 2.0f, 5.0f + 4.0f * randf(), BULDING_SCALE / 2.0f),
            // FIXME: Probably should have more than one base model
            base_model,
        };
        building_types.push_back(building);
    }
    
    // Randomize the building placement
    // FIXME: We might not want the placement to be completely random
    for (int y = 0; y < data.height(); ++y) {
        for (int x = 0; x < data.width(); ++x) {
            data.set(x, y, rand_max(building_types.size()));
        }
    }
}

void City::draw(Renderer* renderer, glm::vec3 cameraPosition) const {
    const glm::vec3 offset = glm::vec3(
        center.x - static_cast<float>(data.width()) * (BULDING_SCALE + BULDING_OFFSET) / 2.0f,
        0,
        center.y - static_cast<float>(data.height()) * (BULDING_SCALE + BULDING_OFFSET) / 2.0f);

    // FIXME: Need to support roads and other features
    for (int y = 0; y < data.height() + (int)cameraPosition.y/4 ; ++y) {
        for (int x = 0; x < data.width() + (int)cameraPosition.y/4 ; ++x) {
            
            //FIXME: camera moves slightly faster then buildings are created
            const glm::vec3 position = glm::vec3(static_cast<float>( (x + (int)cameraPosition.x/4)) * (BULDING_SCALE + BULDING_OFFSET),
                0,
                static_cast<float>( (y + (int)cameraPosition.z/4))  * (BULDING_SCALE + BULDING_OFFSET)) + offset;

            srand( noise(position.x, position.z) ) ;

            const BuildingData building = building_types[data.at(rand_max(data.width()), rand_max(data.height()))];

            const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR,
                  building.scale).transformationMatrix();

            renderer->drawModel(building.model, transform);
        }
    }
}

float City::noise(int x, int y) const {
    int n = x + y * 57;
     n = (n << 13) ^ n;
    return ( 1.0 - (n * (n * n * 15731 + 789221) + 1376312589) / 1073741824.0) + 2;    
}