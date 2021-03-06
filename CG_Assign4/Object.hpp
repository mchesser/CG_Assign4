//! A generic object in 3D space.
#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

class Object {
public:
    /// <summary>
    ///	Create a new object.
    /// Note: Normally it is better to use the default parameters for constructing the object,
    /// then use rotate or lookAt to set the correct position of the object.
    /// </summary>
    ///
    /// <param name="position">The position of the object in the world.</param>
    /// <param name="direction">The direction the object is facing.</param>
    /// <param name="up">The up direction of the object.</param>
    /// <param name="scale">The scale of the object.</param>
    Object(glm::vec3 position,
        glm::vec3 direction = glm::vec3(0, 0, 1),
        glm::vec3 up = glm::vec3(0, 1, 0),
        glm::vec3 scale = glm::vec3(1.0f));

    virtual ~Object() {}

    /// <summary>
    /// Rotates the object around it's center.
    /// </summary>
    ///
    /// <param name="rotation">A vec3 of the form (x angle, y angle, z angle).</param>
    void rotate(glm::vec3 rotation);

    /// <summary>
    /// Rotates the object around a point in space.
    /// The rotation will not affect the object's orientation.
    /// </summary>
    ///
    /// <param name="rotation">A vec3 of the form (x angle, y angle, z angle).</param>
    /// <param name="point">The point to rotate around.</param>
    void rotateAroundPoint(glm::vec3 rotation, glm::vec3 point);

    /// <summary>
    /// Rotate the object so it faces a point.
    /// </summary>
    ///
    /// <param name="point">The target point to face.</param>
    void lookAt(glm::vec3 point);

    /// <summary>
    /// Moves the object with respect to the direction the object is facing.
    /// </summary>
    ///
    /// <param name="amount">The amount to move.</param>
    void move(glm::vec3 amount);

    /// <summary>
    /// Moves the object in world space.
    /// </summary>
    ///
    /// <param name="amount">The amount to move.</param>
    void moveAbsolute(glm::vec3 amount);

    /// <summary>
    /// Get the positional impact of movement with respect to the direction the object is facing,
    /// without actually moving the object.
    /// </summary>
    ///
    /// <param name="amount">The amount to move.</param>
    glm::vec3 inspectMovement(glm::vec3 amount);

    /// <summary>
    /// Computes the model transformation matrix for the model.
    /// </summary>
    glm::mat4 transformationMatrix() const;

    /// <summary>
    /// Returns the position of the object.
    /// </summary>
    glm::vec3 getPosition() const;

    /// <summary>
    /// Returns the direction of the object.
    /// </summary>
    glm::vec3 getDirection() const;

protected:
    /// <summary>
    /// The position of the object in the world.
    /// </summary>
    glm::vec3 position;

    /// <summary>
    /// The direction the object is facing.
    /// </summary>
    glm::vec3 direction;

    /// <summary>
    /// The up direction of the object.
    /// </summary>
    glm::vec3 up;

    /// <summary>
    /// The scale of the object.
    /// </summary>
    glm::vec3 scale;
};

