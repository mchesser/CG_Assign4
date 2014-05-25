//! Functions for generating basic geometric shapes
#pragma once

#include "ModelData.hpp"

namespace shapes {
    /// <summary>
    /// Returns a new quad shape.
    /// </summary>
    RawModelData::Shape quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
};

