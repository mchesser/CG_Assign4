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
    /// </summary>
    glm::mat4 viewProjection(glm::vec3 position)  const;

    /// <summary>
    /// The amount of diffuse light created by the sun.
    /// </summary>
    glm::vec3 diffuse() const;

    /// <summary>
    /// The amount of ambient light created by the sun.
    /// </summary>
    glm::vec3 ambient() const;

    /// <summary>
    /// Increase the speed of rotation of the sun
    ///</summary>
    void increaseSpeed();

    /// <summary>
    /// Decrease the speed of rotation of the sun
    /// </summary>
    void decreaseSpeed();

    /// <summary>
    /// Reset the speed of the sun to default
    /// </summary>
    void resetSpeed();

    /// <summary>
    /// Toggle pause on and off
    /// </summary>
    void togglePause();


    /// <summary>
    /// Get the verticle angle of the sun
    /// </summary>
    float getVerticalAngle();

    /// <summary>
    /// Get the horizontal angle of the sun
    /// </summary>
    float getHorizontalAngle();

    /// <summary>
    /// Set the vertical angle of the sun
    /// </summary>
    ///
    /// <param name="angle"> The new vertical angle, between 0 and TAU</param>
    void setVerticalAngle(float angle);

    /// <summary>
    /// Set the horizontal angle of the sun
    /// </summary>
    ///
    /// <param name="angle"> The new vertical angle, between 0 and TAU</param>
    void setHorizontalAngle(float angle);


private:
    float verticalAngle;
    float horizontalAngle;

    float distance;
    glm::mat4 projection;

    int rotate_speed;

    bool paused;
};

