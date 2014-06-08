#include "City.hpp"
#include <stdlib.h>
#include <cmath>
#include <iostream>

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

#define STREETLIGHT_HEIGHT 0.80f
#define STREETLIGHT_POS_DIV 2.9f

float noise(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return ((1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) + 1.0) / 2.0;
}

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

TileType City::tileForPosition(glm::vec3 position) const {
    const int gridx = static_cast<int>(position.x / TILE_SIZE);
    const int gridy = static_cast<int>(position.z / TILE_SIZE);

    return getTile(gridx, gridy);
}

City::City(const ModelData* base_model, const ModelData* streetlight_model, float renderDistance) {
    buildingTypes.reserve(10);
    for (size_t i = 0; i < 10; ++i) {
        ObjectData building = {
            glm::vec3(BUILDING_SCALE / 2.0f, 1.0 * (2.0 + randf()), BUILDING_SCALE / 2.0f),
            base_model,
        };
        buildingTypes.push_back(building);
    }

    //Streetlight
    streetlight.model = streetlight_model;
    streetlight.scale = glm::vec3(0.001, 0.001, 0.001);

    // Compute a grid size such that the buildings will be rendered so that new buildings
    // can't be seen appearing as the camera moves.
    gridSize = static_cast<int>(ceilf(renderDistance * 2 / TILE_SIZE)) + 5;
}

City::City(std::vector <ModelData *> base_models, const ModelData* streetlight_model, float renderDistance) {
    buildingTypes.reserve(base_models.size());
    for (int i = 0; i < base_models.size(); i++) {
        ObjectData building = {
            glm::vec3(BUILDING_SCALE / 2.0f, 1.0 * (1.0 + randf()), BUILDING_SCALE / 2.0f),
            base_models[i],
        };
        buildingTypes.push_back(building);
    }

    //Streetlight
    streetlight.model = streetlight_model;
    streetlight.scale = glm::vec3(0.001, 0.001, 0.001);

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
                const int index = (int)(noise(gridx, gridy) * (buildingTypes.size()));
                const ObjectData building = buildingTypes[index];

                const glm::vec3 position = tileOffset + glm::vec3(0, building.scale.y, 0);

                const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR,
                    building.scale).transformationMatrix();

                renderer->drawModel(building.model, transform);
            }
                break;

            case V: // Vertical road segment
            {
                if (gridy % 2 == 0) {
                    const glm::vec3 position = tileOffset + glm::vec3(-TILE_SIZE/2, 0.01, 0.0);
                    Object arrangement = Object(position, STREET_DIR, SKY_DIR, streetlight.scale);
                    arrangement.rotate(glm::vec3(0.0, TAU/4, 0.0));
                    const glm::mat4 transform = arrangement.transformationMatrix();
                    renderer->drawModel(streetlight.model, transform);

                    renderer->addLight(tileOffset + glm::vec3(-TILE_SIZE / STREETLIGHT_POS_DIV, STREETLIGHT_HEIGHT, 0));
                }
                else {
                    const glm::vec3 position = tileOffset + glm::vec3(TILE_SIZE/2, 0.01, 0.0);
                    Object arrangement = Object(position, STREET_DIR, SKY_DIR, streetlight.scale);
                    arrangement.rotate(glm::vec3(0.0, TAU/-4, 0.0));
                    const glm::mat4 transform = arrangement.transformationMatrix();
                    renderer->drawModel(streetlight.model, transform);

                    renderer->addLight(tileOffset + glm::vec3(TILE_SIZE / STREETLIGHT_POS_DIV, STREETLIGHT_HEIGHT, 0));
                }
            }
                break;
            case H: // Horizontal road segment
            {
                if (gridx % 2 == 0) {
                    const glm::vec3 position = tileOffset + glm::vec3(0.0, 0.01, -TILE_SIZE / 2);
                    const glm::mat4 transform = Object(position, STREET_DIR, SKY_DIR, 
                        streetlight.scale).transformationMatrix();
                    renderer->drawModel(streetlight.model, transform);

                    renderer->addLight(tileOffset + glm::vec3(0, STREETLIGHT_HEIGHT, -TILE_SIZE / STREETLIGHT_POS_DIV));
                }
                else {
                    const glm::vec3 position = tileOffset + glm::vec3(0.0, 0.01, TILE_SIZE / 2);
                    Object arrangement = Object(position, STREET_DIR, SKY_DIR, streetlight.scale);
                    arrangement.rotate(glm::vec3(0.0, TAU/2, 0.0));
                    const glm::mat4 transform = arrangement.transformationMatrix();
                    renderer->drawModel(streetlight.model, transform);

                    renderer->addLight(tileOffset + glm::vec3(0, STREETLIGHT_HEIGHT, TILE_SIZE / STREETLIGHT_POS_DIV));
                }
            }
            }
        }
    }
}
