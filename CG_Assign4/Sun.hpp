//! Class for managing the sun
#pragma once
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class Sun {
public:
    /// <summary>
    /// Create a new sun with the specified angles.
    /// </summary>
    Sun(float verticalAngle, float horizontalAngle);

    /// <summary>
    /// Compute the position of the sun.
    /// </summary>
    glm::vec3 position() const;

    void update(float elapsedSeconds);

    /// <summary>
    /// Compute the view projection matrix, with respect to some position
    /// <summary>
    glm::mat4 viewProjection(glm::vec3 position)  const;

    /// <summary>
    /// The amount of diffuse light created by the sun.
    /// <summary>
    glm::vec3 diffuse() const;

    /// <summary>
    /// The amount of ambient light created by the sun.
    /// <summary>
    glm::vec3 ambient() const;

private:
    float verticalAngle;
    float horizontalAngle;

    float distance;
    glm::mat4 projection;
};

