#include "Shapes.hpp"
#include "glm/vec2.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

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

        // Compute Tangents
        shape.tangents.resize(shape.vertices.size(), glm::vec3(0.0,0.0,0.0));

        for (int i = 0; i < shape.indices.size(); i+=3) {
            int i1 = shape.indices.at(i);
            int i2 = shape.indices.at(i+1);
            int i3 = shape.indices.at(i+2);
            glm::vec3 point1 = shape.vertices.at(i1);
            glm::vec3 point2 = shape.vertices.at(i2);
            glm::vec3 point3 = shape.vertices.at(i3);
            glm::vec2 uv1 = shape.texCoords.at(i1);
            glm::vec2 uv2 = shape.texCoords.at(i2);
            glm::vec2 uv3 = shape.texCoords.at(i3);

            glm::vec3 p1p2 = point2 - point1;
            glm::vec3 p1p3 = point3 - point1;
            glm::vec2 uv1uv2 = uv2 - uv1;
            glm::vec2 uv1uv3 = uv3 - uv1;

            float c = uv1uv2.s * uv1uv3.t - uv1uv3.s * uv1uv2.t;
            if (c != 0) {
                float mul = 1.0 / c;
                glm::vec3 tangent = (p1p2 * uv1uv3.t - p1p3 * uv1uv2.t) * mul;
                shape.tangents.at(i1) = glm::normalize(tangent);
                shape.tangents.at(i2) = glm::normalize(tangent);
                shape.tangents.at(i3) = glm::normalize(tangent);
            }
        }

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
            shape.normals.push_back(-normal);
        }

        shape.texCoords.reserve(3);
        shape.texCoords.push_back(glm::vec2(0, 1));
        shape.texCoords.push_back(glm::vec2(0, 0));
        shape.texCoords.push_back(glm::vec2(1, 0));

        shape.indices.push_back(0);
        shape.indices.push_back(2);
        shape.indices.push_back(1);

        shape.material = DEFAULT_MATERIAL;

        shape.textureName = "";

        return shape;
    }
};