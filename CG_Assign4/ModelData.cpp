#include "ModelData.hpp"
#include "tiny_obj_loader/tiny_obj_loader.h"
#include "AssetManager.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

// Reads a glm::vec3 from an array of floats
#define READ_VEC3(v) (glm::vec3((v)[0], (v)[1], (v)[2]))
#define READ_VEC2(v) (glm::vec2((v)[0], (v)[1]))

// Gets the address of the elements of a vector
template<class T>
static GLvoid** dataPtr(const std::vector<T>& vec) {
    return (GLvoid**)(&vec[0]);
}

RawModelData loadModelData(const std::string& filename, bool opposite_winding) {
    // --------------------------------------------------
    // Load the base model using the tinyobjreader library
    // --------------------------------------------------

    std::vector<tinyobj::shape_t> baseShapes;
    std::string subdir = filename.substr(0, filename.find_last_of('/') + 1);
    std::string err = tinyobj::LoadObj(baseShapes, filename.c_str(), subdir.c_str());

    // Check the model loaded correctly
    if (!err.empty()) {
        std::cerr << err << std::endl;
        exit(EXIT_FAILURE);
    }

    // --------------------------------------------------------------
    // Compute normals and rearrange the structure of the loaded data
    // --------------------------------------------------------------

    RawModelData data;
    for (size_t i = 0; i < baseShapes.size(); ++i) {
        RawModelData::Shape shape;
        shape.vertices.reserve(baseShapes[i].mesh.indices.size());
        shape.normals.reserve(baseShapes[i].mesh.indices.size());
        shape.texCoords.reserve(baseShapes[i].mesh.indices.size());

        for (size_t j = 0; j < baseShapes[i].mesh.indices.size(); j += 3) {
            const unsigned int i1 = baseShapes[i].mesh.indices[j + 0];
            const unsigned int i2 = baseShapes[i].mesh.indices[j + 1];
            const unsigned int i3 = baseShapes[i].mesh.indices[j + 2];

            const glm::vec3 a = READ_VEC3(&baseShapes[i].mesh.positions[i1 * 3]);
            const glm::vec3 b = READ_VEC3(&baseShapes[i].mesh.positions[i2 * 3]);
            const glm::vec3 c = READ_VEC3(&baseShapes[i].mesh.positions[i3 * 3]);

            shape.vertices.push_back(a);
            shape.vertices.push_back(b);
            shape.vertices.push_back(c);

            glm::vec3 normal;
            if (!opposite_winding) {
                normal = glm::normalize(glm::cross(b - a, c - a));
            }
            else {
                normal = glm::normalize(glm::cross(c - a, b - a));
            }
            for (int n = 0; n < 3; ++n) {
                shape.normals.push_back(normal);
            }

            if (baseShapes[i].mesh.texcoords.size() > 0) {
                shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i1 * 2])));
                shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i2 * 2])));
                shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i3 * 2])));
            }
            if (!opposite_winding) {
                shape.indices.push_back(static_cast<unsigned int>(j + 0));
                shape.indices.push_back(static_cast<unsigned int>(j + 1));
                shape.indices.push_back(static_cast<unsigned int>(j + 2));
            }
            else {
                shape.indices.push_back(static_cast<unsigned int>(j + 0));
                shape.indices.push_back(static_cast<unsigned int>(j + 2));
                shape.indices.push_back(static_cast<unsigned int>(j + 1));
            }
        }

        // Read the material properties
        shape.material.ambient = READ_VEC3(baseShapes[i].material.ambient);
        shape.material.diffuse = READ_VEC3(baseShapes[i].material.diffuse);
        shape.material.specular = READ_VEC3(baseShapes[i].material.specular);
        shape.material.shininess = baseShapes[i].material.shininess;
        shape.material.dissolve = baseShapes[i].material.dissolve;

        if (!baseShapes[i].material.diffuse_texname.empty()) {
            shape.textureName = subdir + baseShapes[i].material.diffuse_texname;
            // The filename from tiny_obj_loader does not trim trailing whitespace. This causes issues
            // when using SOIL to load textures on Linux. So we need to manually trim whitespace 
            // It is unfortunate that C++ does not have a nice way for trimming strings, so we need
            // to use this complicated expression.
            shape.textureName.erase(shape.textureName.find_last_not_of(" \n\r\t") + 1);
        }

        data.shapes.push_back(shape);
    }
    return data;
}

ModelData::ModelData(const RawModelData& data, const Renderer* renderer) {
    unsigned int totalAttributes = 0;
    unsigned int totalElements = 0;

    std::vector<unsigned int> indices;
    for (size_t i = 0; i < data.shapes.size(); ++i) {
        // Recalculate indices based on the position in the buffer
        for (size_t j = 0; j < data.shapes[i].indices.size(); ++j) {
            indices.push_back(data.shapes[i].indices[j] + totalAttributes);
        }

        totalAttributes += data.shapes[i].vertices.size();
        totalElements += data.shapes[i].indices.size();
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(4, buffers);

    // Allocate memory for buffers
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, totalAttributes * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(renderer->shader.in_coord);
    glVertexAttribPointer(renderer->shader.in_coord, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, totalAttributes * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(renderer->shader.in_normal);
    glVertexAttribPointer(renderer->shader.in_normal, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, totalAttributes * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(renderer->shader.in_texcoord);
    glVertexAttribPointer(renderer->shader.in_texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalElements * sizeof(unsigned int), dataPtr(indices), GL_STATIC_DRAW);

    unsigned int attributeArrayOffset = 0;
    unsigned int elementArrayOffset = 0;
    for (size_t i = 0; i < data.shapes.size(); ++i) {
        Shape shape;
        unsigned int attributeArraySize = data.shapes[i].vertices.size();
        unsigned int elementArraySize = data.shapes[i].indices.size();

        // Load vertices into the buffer
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferSubData(GL_ARRAY_BUFFER, attributeArrayOffset * sizeof(glm::vec3),
            attributeArraySize * sizeof(glm::vec3), dataPtr(data.shapes[i].vertices));

        // Load normals into the buffer
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferSubData(GL_ARRAY_BUFFER, attributeArrayOffset * sizeof(glm::vec3),
            attributeArraySize * sizeof(glm::vec3), dataPtr(data.shapes[i].normals));

        // Load texture coordinates into the buffer
        if (data.shapes[i].texCoords.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
            glBufferSubData(GL_ARRAY_BUFFER, attributeArrayOffset * sizeof(glm::vec2),
                attributeArraySize * sizeof(glm::vec2), dataPtr(data.shapes[i].texCoords));
        }

        // Load the texture using SOIL
        if (!data.shapes[i].textureName.empty()) {
            shape.textureId = AssetManager::loadTexture(data.shapes[i].textureName);
        }

        shape.elementOffset = elementArrayOffset * sizeof(unsigned int);
        shape.numElements = data.shapes[i].indices.size();
        shape.material = data.shapes[i].material;
        shapes.push_back(shape);

        attributeArrayOffset += attributeArraySize;
        elementArrayOffset += elementArraySize;
    }

    boundingBox.minVertex = data.boundingBox.minVertex;
    boundingBox.maxVertex = data.boundingBox.maxVertex;
}

ModelData::~ModelData() {
    glDeleteBuffers(4, buffers);
    glDeleteVertexArrays(1, &vao);
}

void ModelData::unify() {
    if (shapes.size() > 1) {
        unsigned int totalElements = 0;
        for (size_t i = 0; i < shapes.size(); ++i) {
            totalElements += shapes[i].numElements;
        }
        shapes[0].numElements = totalElements;
        shapes.resize(1);
    }
}

void ModelData::reduce() {
    std::vector<Shape> newShapes;
    unsigned int offset = 0;

    size_t i = 0;
    while (i < shapes.size()) {
        unsigned int numElements = 0;
        size_t j = i + 1;
        while (j < shapes.size() && shapes[j].textureId == shapes[i].textureId) {
            numElements += shapes[j].numElements;
            j += 1;
        }

        Shape newShape = shapes[i];
        newShape.numElements += numElements;
        newShapes.push_back(newShape);
        offset += numElements * sizeof(unsigned int);

        i = j;
    }

    shapes = newShapes;
}