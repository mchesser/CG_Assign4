#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position)
    : Object(position) {
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position, position + direction, up);
}