//! Generates tiled ground
#pragma once

#include "Shapes.hpp"
#include "City.hpp"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

class Terrain
{
public:
	/// <summary>
    /// Creates all tile models
    /// </summary>
	Terrain(Renderer* renderer);

	/// <summary>
    /// Draws a grid of terrain models corresponding to the city grid
    /// </summary>
	void draw(Renderer* renderer, City* city, glm::vec3 cameraPosition, int size) const;
private:
	ModelData* horizontalRoad;
	ModelData* verticalRoad;
	ModelData* intersection;
	ModelData* building;

};