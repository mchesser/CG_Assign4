//! A simple controllable camera class.
#pragma once
#include "Object.hpp"
#include "glm/mat4x4.hpp"

class Camera : public Object {
public:
    /// <summary>
    /// Create a new Camera.
    /// </summary>
    ///
    /// <param name="position">The position of the camera in the world.</param>
    /// <param name="target">The position of the camera's target.</param>
    Camera(glm::vec3 position, glm::vec3 target);

    /// <summary> 
    /// Move the camera's target by a specified amount.
    /// </summary>
    ///
    /// <param name="amount">The amount to move the target by.</param>
    void moveTarget(glm::vec3 amount);

    /// <summary>
    /// Construct a view transformation from the camera.
    /// </summary>
    glm::mat4 view() const;
};

