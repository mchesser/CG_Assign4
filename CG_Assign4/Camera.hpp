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
    Camera(glm::vec3 position);

    /// <summary>
    /// Construct a view transformation from the camera.
    /// </summary>
    glm::mat4 view() const;
};

