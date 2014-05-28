#include "City.hpp"
#include <stdlib.h>

// FIXME: This should be dynamic
#define BUILDING_SCALE (2.0f)
// FIXME: This should by dynamic
#define BUILDING_GAP (2.0f)
#define BUILDING_OFFSET (BUILDING_SCALE * BUILDING_GAP)

#define NUM_BUILDINGS (30)

// FIXME: if we want buildings to have a unique street facing facade this value needs to be
// dynamic
#define STREET_DIR (glm::vec3(0, 0, 1))
#define SKY_DIR (glm::vec3(0, 1, 0))

// FIXME: Very poor way of generating a bounded random int
#define rand_max(max) (rand() % max)
// FIXME: Very poor way of generating a random float
#define randf() (static_cast<float>(rand()) / RAND_MAX)

float noise(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return ((1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) + 1.0) / 2.0;
}

City::City(const ModelData* base_model) {
    // Generate 10 buildings of different heights
    // FIXME: There might be other ways of varying the buildings
    building_types.reserve(10);
    for (size_t i = 0; i < 10; ++i) {
        BuildingData building = {
            glm::vec3(BUILDING_SCALE / 2.0f, 1.0f + 4.0f * randf(), BUILDING_SCALE / 2.0f),
            // FIXME: Probably should have more than one base model
            base_model,
        };
        building_types.push_back(building);
    }
}

void City::draw(Renderer* renderer, glm::vec3 cameraPosition) const {
    const glm::vec3 offset = glm::vec3(
        -static_cast<float>(NUM_BUILDINGS) * BUILDING_OFFSET / 2.0f,
        0,
        -static_cast<float>(NUM_BUILDINGS) * BUILDING_OFFSET / 2.0f);

    // FIXME: Need to support roads and other features
    for (int y = 0; y < NUM_BUILDINGS; ++y) {
        for (int x = 0; x < NUM_BUILDINGS; ++x) {
            const int worldx = x + static_cast<int>(cameraPosition.x) / 4;
            const int worldy = y + (int)cameraPosition.z / 4;

            const int index = (int)(noise(worldx, worldy) * (float)(building_types.size()));
            const BuildingData building = building_types[index];

            //FIXME: camera moves slightly faster then buildings are created
            const glm::vec3 position = glm::vec3(static_cast<float>(worldx) * BUILDING_OFFSET,
                building.scale.y,
                static_cast<float>(worldy) * BUILDING_OFFSET) + offset;

            const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR,
                building.scale).transformationMatrix();

            renderer->drawModel(building.model, transform);
        }
    }
}