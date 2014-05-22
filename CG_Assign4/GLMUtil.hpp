//! Simple utility functions for GLM
#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

/// <summary>
/// A utility function for obtaining a matrix that rotates a point with respect to the
/// axis specified
/// </summary>
///
/// <param name="rotation">A vec3 of the form (x angle, y angle, z angle).</param>
/// <param name="xAxis">A unit vector in the direction of the x axis.</param>
/// <param name="yAxis">A unit vector in the direction of the y axis.</param>
/// <param name="zAxis">A unit vector in the direction of the z axis.</param>
inline glm::mat4 rotate3D(glm::vec3 rotation, glm::vec3 xAxis, glm::vec3 yAxis, glm::vec3 zAxis) {
    return (
        glm::rotate(glm::mat4(1.0f), rotation.x, xAxis) *
        glm::rotate(glm::mat4(1.0f), rotation.y, yAxis) *
        glm::rotate(glm::mat4(1.0f), rotation.z, zAxis)
        );
}

/// <summary>
/// A utility function for obtaining a matrix that rotates a point with respect to the
/// axis (1, 0, 0), (0, 1, 0), (0, 0, 1)
/// </summary>
///
/// <param name="rotation">A vec3 of the form (x angle, y angle, z angle).</param>
inline glm::mat4 rotate3D(glm::vec3 rotation) {
    return rotate3D(rotation, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
}