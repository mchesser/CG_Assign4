#include "Sun.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

#define TAU (6.283185307179586f)
#define DEFAULT_ROTATE_SPEED (100) // seconds
#define UPDATE_SPEED (5)
#define MIN_SPEED (500)
#define MAX_SPEED (5)

glm::vec3 linear_color_gradient(const glm::vec3 colors[], size_t numColors, float x) {
    // Handle the the cases where there is only one color to sample
    if (x <= 0.0) {
        return colors[0];
    }
    else if (x >= 1.0) {
        return colors[numColors-1];
    }

    const float band_width = numColors - 1.0;
    const int left_color = x * band_width;
    const int right_color = left_color + 1;
    const float x_new = x * band_width - left_color;

    // Linearly interpolate between the two colors
    return colors[left_color] + x_new * (colors[right_color] - colors[left_color]);
}

Sun::Sun(float verticalAngle, float horizontalAngle) 
    : verticalAngle(verticalAngle), horizontalAngle(horizontalAngle), 
      rotate_speed(DEFAULT_ROTATE_SPEED), paused(false) {
    
    distance = 700;
    projection = glm::ortho<float>(-50, 50, -50, 50, distance - 50, distance + 50);
}

void Sun::update(float elapsedSeconds) {
    if (paused == false) {
        verticalAngle += elapsedSeconds * TAU / rotate_speed;
        if (verticalAngle > TAU) {
            verticalAngle -= TAU;
        }
    }
}

glm::vec3 Sun::position() const {
    return distance * glm::normalize(glm::vec3(
        glm::cos(verticalAngle) * glm::cos(horizontalAngle),
        glm::sin(verticalAngle),
        glm::cos(verticalAngle) * glm::sin(horizontalAngle)
    ));
}

glm::mat4 Sun::viewProjection(glm::vec3 position) const {
    return projection *
        glm::lookAt(position + this->position(), position, glm::vec3(0, 1, 0));
}

void Sun::increaseSpeed() {
    if (rotate_speed - UPDATE_SPEED > MAX_SPEED)
        rotate_speed -= UPDATE_SPEED;
}

void Sun::decreaseSpeed() {
    if (rotate_speed + UPDATE_SPEED < MIN_SPEED)
        rotate_speed += UPDATE_SPEED;
}

void Sun::togglePause() {
    paused = !paused;
}

float Sun::getVerticalAngle() {
    return verticalAngle;
}

float Sun::getHorizontalAngle() {
    return horizontalAngle;
}

glm::vec3 Sun::diffuse() const { 
    static const glm::vec3 colors[3] = {
        glm::vec3(1), // Day
        glm::vec3(1.0f, 0.6f, 0.4f), // Sunset
        glm::vec3(0) // Night
    };

    // FIXME: It makes more sense to calculate the light using the angle of the sun
    // rather than the position.
    const glm::vec3 sun_position = position();
    if (sun_position.y < 0.0f) {
        return colors[2];
    }
    else if (sun_position.y < 200.0f) {
        float x = sun_position.y / 200.0f;
        return linear_color_gradient(colors, 3, 1.0 - x);
    }
    return colors[0];
}

glm::vec3 Sun::ambient() const {
    static const glm::vec3 colors[2] = {
        glm::vec3(0.4f), // Day
        glm::vec3(0.1f, 0.1f, 0.3f) // Night
    };

    // FIXME: It makes more sense to calculate the light using the angle of the sun
    // rather than the position.
    const glm::vec3 sun_position = position();

    if (sun_position.y > 0) {
        return colors[0];
    }
    else if (sun_position.y > -200.0f) {
        float x = sun_position.y / -200.0f;
        return linear_color_gradient(colors, 2, x);
    }
    return colors[1];
}