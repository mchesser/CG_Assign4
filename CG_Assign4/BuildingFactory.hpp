//! A class to create random rawModelData buildings
#pragma once

#include "Shapes.hpp"
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

class BuildingFactory
{
public:
	/// <summary>
    /// Returns a block styled building
    /// </summary>
	RawModelData genBlockBuilding(const std::string& texture);

	/// <summary>
    /// Returns a number of random generated buildings
    /// </summary>
	std::vector <RawModelData> genBuildings(const std::string& texture, int number);

};