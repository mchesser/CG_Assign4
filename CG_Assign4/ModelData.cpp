#include "ModelData.hpp"
#include "tiny_obj_loader/tiny_obj_loader.h"
#include "SOIL/SOIL.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

// Utility macros for determining the size of buffers
#define BUFFER_SIZE_3(n) (n * sizeof(glm::vec3))
#define BUFFER_SIZE_2(n) (n * sizeof(glm::vec2))

// Reads a glm::vec3 from an array of floats
#define READ_VEC3(v) (glm::vec3((v)[0], (v)[1], (v)[2]))
#define READ_VEC2(v) (glm::vec2((v)[0], (v)[1]))

// Gets the address of the elements of a vector
template<class T>
static GLvoid** dataPtr(const std::vector<T>& vec) {
    return (GLvoid**)(&vec[0]);
}

RawModelData loadModelData(const std::string& filename) {
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

    // --------------------------------------------------
    // Compute normals and rearrange the structure of the loaded data
    // --------------------------------------------------

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

            const glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
            for (int n = 0; n < 3; ++n) {
                shape.normals.push_back(normal);
            }

            shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i1 * 2])));
            shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i2 * 2])));
            shape.texCoords.push_back(READ_VEC2(&(baseShapes[i].mesh.texcoords[i3 * 2])));

            shape.indices.push_back(static_cast<unsigned int>(j + 0));
            shape.indices.push_back(static_cast<unsigned int>(j + 1));
            shape.indices.push_back(static_cast<unsigned int>(j + 2));
        }

        // Read the material properties
        shape.material.ambient = READ_VEC3(baseShapes[i].material.ambient);
        shape.material.diffuse = READ_VEC3(baseShapes[i].material.diffuse);
        shape.material.specular = READ_VEC3(baseShapes[i].material.specular);
        shape.material.shininess = baseShapes[i].material.shininess;
        shape.material.dissolve = baseShapes[i].material.dissolve;

        shape.textureName = subdir + baseShapes[i].material.diffuse_texname;
        // The filename from tiny_obj_loader does not trim trailing whitespace. This causes issues
        // when using SOIL to load textures on Linux. So we need to manually trim whitespace 
        // It is unfortunate that C++ does not have a nice way for trimming strings, so we need
        // to use this complicated expression.
        shape.textureName.erase(shape.textureName.find_last_not_of(" \n\r\t") + 1);

        data.shapes.push_back(shape);
    }
    return data;
}

ModelData::ModelData(const RawModelData& data, const Renderer* renderer) {
    for (size_t i = 0; i < data.shapes.size(); ++i) {
        Shape shape;
        glGenVertexArrays(1, &shape.vao);
        glBindVertexArray(shape.vao);

        glGenBuffers(4, shape.buffers);

        // Load vertices into a buffer
        glBindBuffer(GL_ARRAY_BUFFER, shape.buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE_3(data.shapes[i].vertices.size()),
            dataPtr(data.shapes[i].vertices), GL_STATIC_DRAW);
        glEnableVertexAttribArray(renderer->shader.in_coord);
        glVertexAttribPointer(renderer->shader.in_coord, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Load normals into a buffer
        glBindBuffer(GL_ARRAY_BUFFER, shape.buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE_3(data.shapes[i].normals.size()),
            dataPtr(data.shapes[i].normals), GL_STATIC_DRAW);
        glEnableVertexAttribArray(renderer->shader.in_normal);
        glVertexAttribPointer(renderer->shader.in_normal, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Load texture coordinates into a buffer
        glBindBuffer(GL_ARRAY_BUFFER, shape.buffers[2]);
        glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE_2(data.shapes[i].texCoords.size()),
            dataPtr(data.shapes[i].texCoords), GL_STATIC_DRAW);
        glEnableVertexAttribArray(renderer->shader.in_texcoord);
        glVertexAttribPointer(renderer->shader.in_texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Load the texture using SOIL
        shape.textureId = SOIL_load_OGL_texture(data.shapes[i].textureName.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

        // Load indices into a buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.buffers[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.shapes[i].indices.size() * sizeof(unsigned int),
            dataPtr(data.shapes[i].indices), GL_STATIC_DRAW);

        shape.numElements = data.shapes[i].indices.size();
        shape.material = data.shapes[i].material;

        shapes.push_back(shape);
    }
}

ModelData::~ModelData() {
    for (size_t i = 0; i < shapes.size(); ++i) {
        glDeleteBuffers(4, shapes[i].buffers);
        glDeleteTextures(1, &shapes[i].textureId);
        glDeleteVertexArrays(1, &shapes[i].vao);
    }
}