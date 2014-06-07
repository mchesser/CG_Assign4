#include "Shapes.hpp"
#include "glm/vec2.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace shapes {
    RawModelData::Shape quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
        RawModelData::Shape shape;

        shape.vertices.reserve(4);
        shape.vertices.push_back(p1);
        shape.vertices.push_back(p2);
        shape.vertices.push_back(p3);
        shape.vertices.push_back(p4);

        shape.normals.reserve(4);
        const glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
        for (size_t i = 0; i < 4; ++i) {
            shape.normals.push_back(normal);
        }

        shape.texCoords.reserve(4);
        shape.texCoords.push_back(glm::vec2(0, 1));
        shape.texCoords.push_back(glm::vec2(0, 0));
        shape.texCoords.push_back(glm::vec2(1, 0));
        shape.texCoords.push_back(glm::vec2(1, 1));

        shape.indices.push_back(0);
        shape.indices.push_back(1);
        shape.indices.push_back(2);
        shape.indices.push_back(0);
        shape.indices.push_back(2);
        shape.indices.push_back(3);

        shape.material = DEFAULT_MATERIAL;

        shape.textureName = "";

        return shape;
    }

    RawModelData::Shape triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        RawModelData::Shape shape;

        shape.vertices.reserve(3);
        shape.vertices.push_back(p1);
        shape.vertices.push_back(p2);
        shape.vertices.push_back(p3);

        shape.normals.reserve(3);
        const glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
        for (size_t i = 0; i < 3; ++i) {
            shape.normals.push_back(normal);
        }

        shape.texCoords.reserve(3);
        shape.texCoords.push_back(glm::vec2(0, 1));
        shape.texCoords.push_back(glm::vec2(0, 0));
        shape.texCoords.push_back(glm::vec2(1, 0));

        shape.indices.push_back(0);
        shape.indices.push_back(1);
        shape.indices.push_back(2);

        shape.material = DEFAULT_MATERIAL;

        shape.textureName = "";

        return shape;
    }
};