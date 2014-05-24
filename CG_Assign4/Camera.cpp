#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 target)
    : Object(position) {
    lookAt(target);
}

void Camera::moveTarget(glm::vec3 amount) {
    const glm::vec3 new_target = (position + direction) + amount.x * glm::cross(direction, up) +
        amount.y * up + amount.z * direction;
    lookAt(new_target);
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position, position + direction, up);
}