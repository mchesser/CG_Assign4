#include "City.hpp"
#include <stdlib.h>
#include <cmath>

#define TAU (6.283185307179586f)

// FIXME: This should be dynamic
#define BUILDING_SCALE (2.0f)
// FIXME: This should by dynamic
#define BUILDING_GAP (1.2f)
#define TILE_SIZE (BUILDING_SCALE * BUILDING_GAP)

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

TileType getTile(int x, int y) {
    x = x < 0 ? KEY_WIDTH - 1 + ((x + 1) % KEY_WIDTH) : x % KEY_WIDTH;
    y = y < 0 ? KEY_HEIGHT - 1 + ((y + 1) % KEY_HEIGHT) : y % KEY_HEIGHT;

    return TILE_KEY[y][x];
}

City::City(const ModelData* base_model, float renderDistance) {
    // Generate 10 buildings of different heights
    // FIXME: There might be other ways of varying the buildings
    buildingTypes.reserve(10);
    for (size_t i = 0; i < 10; ++i) {
        BuildingData building = {
            glm::vec3(BUILDING_SCALE / 2.0f, 1.0, BUILDING_SCALE / 2.0f), //1.0f + 4.0f * randf()
            // FIXME: Probably should have more than one base model
            base_model,
        };
        buildingTypes.push_back(building);
    }

    // Compute a grid size such that the buildings will be rendered so that new buildings
    // can't be seen appearing as the camera moves.
    gridSize = static_cast<int>(ceilf(renderDistance * 2 / TILE_SIZE)) + 5;
}

City::City(std::vector <ModelData *> base_models, float renderDistance) {
    for (int i = 0; i< base_models.size(); i++) {
        BuildingData building = {
            glm::vec3(BUILDING_SCALE / 2.0f, 1.0, BUILDING_SCALE / 2.0f),
            base_models[i]e,
        };
        buildingTypes.push_back(building);
    }

    // Compute a grid size such that the buildings will be rendered so that new buildings
    // can't be seen appearing as the camera moves.
    gridSize = static_cast<int>(ceilf(renderDistance * 2 / TILE_SIZE)) + 5;
}

void City::draw(Renderer* renderer, glm::vec3 cameraPosition) const {
    const glm::vec3 baseOffset = glm::vec3(
        -static_cast<float>(gridSize)* TILE_SIZE / 2.0f,
        0,
        -static_cast<float>(gridSize)* TILE_SIZE / 2.0f);

    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x) {
            const int gridx = x + static_cast<int>(cameraPosition.x / TILE_SIZE);
            const int gridy = y + static_cast<int>(cameraPosition.z / TILE_SIZE);

            const glm::vec3 tileOffset = glm::vec3(static_cast<float>(gridx)* TILE_SIZE,
                0,
                static_cast<float>(gridy)* TILE_SIZE) + baseOffset;

            switch (getTile(gridx, gridy)) {
            case B: // Building case
            {
                // Get the random building model from the array
                const int index = (int)(noise(gridx, gridy) * (float)(buildingTypes.size()));
                const BuildingData building = buildingTypes[index];

                const glm::vec3 position = tileOffset + glm::vec3(0, building.scale.y, 0);

                const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR,
                    building.scale).transformationMatrix();

                renderer->drawModel(building.model, transform);
            }
                break;

            case V: // Vertical road segment
            {
                if (gridy % 2 == 0) {
                    renderer->addLight(tileOffset + glm::vec3(-TILE_SIZE / 4, 3, 0));
                }
                else {
                    renderer->addLight(tileOffset + glm::vec3(TILE_SIZE / 4, 3, 0));
                }
            }
                break;
            case H: // Horizontal road segment
            {
                if (gridx % 2 == 0) {
                    renderer->addLight(tileOffset + glm::vec3(0, 3, -TILE_SIZE / 4));
                }
                else {
                    renderer->addLight(tileOffset + glm::vec3(0, 3, TILE_SIZE / 4));
                }
            }
            }
        }
    }
}