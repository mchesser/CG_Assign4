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
    /// Draws the city.
    /// </summary>
    ///
    /// <param name="renderer>The renderer to draw to.</renderer>
    void draw(const Renderer* renderer) const;

private:
    std::vector<BuildingData> building_types;
    Array2D<int> data;
    glm::vec2 center;
};

