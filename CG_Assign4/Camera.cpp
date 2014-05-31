#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 target)
    : Object(position) {
    lookAt(target);
}

void Camera::moveTarget(glm::vec3 amount) {
    const glm::vec3 newTarget = (position + direction) + amount.x * glm::cross(direction, up) +
        amount.y * up + amount.z * direction;
    if (newTarget == position) {
        // If the target it equal to the position then nothing should happen
        return;
    }
    lookAt(newTarget);
    up = glm::vec3(0, 1, 0);
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position, position + direction, up);
}