#include "City.hpp"
#include <stdlib.h>

// FIXME: This should be dynamic
#define BUILDING_SCALE (2.0f)
// FIXME: This should by dynamic
#define BUILDING_GAP (1.2f)
#define TILE_SIZE (BUILDING_SCALE * BUILDING_GAP)

#define GRID_SIZE (50)

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

enum TileType {
    H, // Horizontal road
    V, // Vertical road
    I, // Intersection road
    B, // Building
};

#define KEY_WIDTH 5
#define KEY_HEIGHT 6

const static TileType TILE_KEY[KEY_HEIGHT][KEY_WIDTH] = {
    { B, B, V, B, B },
    { H, H, I, H, H },
    { B, B, V, B, B },
    { B, B, V, B, B },
    { H, H, I, H, H },
    { B, B, V, B, B },
};

#include <iostream>


TileType getTile(int x, int y) {
    x = x < 0 ? KEY_WIDTH-1 + (x % KEY_WIDTH) : x % KEY_WIDTH;
    y = y < 0 ? KEY_HEIGHT-1 + (y % KEY_HEIGHT): y % KEY_HEIGHT;

    return TILE_KEY[y][x];
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
        -static_cast<float>(GRID_SIZE) * TILE_SIZE / 2.0f,
        0,
        -static_cast<float>(GRID_SIZE) * TILE_SIZE / 2.0f);

    // FIXME: Need to support roads and other features
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            const int gridx = x + static_cast<int>(cameraPosition.x / TILE_SIZE);
            const int gridy = y + static_cast<int>(cameraPosition.z / TILE_SIZE);

            if (getTile(gridx, gridy) == B) {
                const int index = (int)(noise(gridx, gridy) * (float)(building_types.size()));
                const BuildingData building = building_types[index];

                const glm::vec3 position = glm::vec3(static_cast<float>(gridx)* TILE_SIZE,
                    building.scale.y,
                    static_cast<float>(gridy)* TILE_SIZE) + offset;

                const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR,
                    building.scale).transformationMatrix();

                renderer->drawModel(building.model, transform);
            }
        }
    }
}