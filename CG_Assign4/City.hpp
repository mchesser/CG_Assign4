//! A basic class for creating, storing and rendering a basic city
#pragma once
#include "Renderer.hpp"
#include "ModelData.hpp"
#include "Array2d.hpp"
#include "glm/vec2.hpp"

struct BuildingData {
    glm::vec3 scale;
    const ModelData* model;
};


class City {
public:
    /// <summary>
    /// Creates a new city.
    /// </summary>
    City(const ModelData* base_model);

    /// <summary>
    /// Checks for collision at location x, y.
    /// </summary>
    bool checkCollision(glm::vec3 position);

    /// <summary>
    /// Draws the city.
    /// </summary>
    ///
    /// <param name="renderer>The renderer to draw to.</renderer>
    void draw(Renderer* renderer, glm::vec3 cameraPosition) const;

private:
    std::vector<BuildingData> building_types;
};

