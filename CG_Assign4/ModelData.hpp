//! Class for managing model data
#pragma once
#include <string>
#include <vector>
#include "Object.hpp"
#include "GLHeaders.hpp"
#include "Renderer.hpp"

class Renderer;

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float dissolve;
};

const Material DEFAULT_MATERIAL = {
    glm::vec3(0.3f),
    glm::vec3(1.0f),
    glm::vec3(0.1f),
    0.0f,
    0.0f
};

struct BoundingBox {
    glm::vec3 maxVertex;
    glm::vec3 minVertex;
};

struct RawModelData {
    BoundingBox boundingBox;

    struct Shape {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<unsigned int> indices;
        Material material;
        std::string textureName;
    };
    std::vector<Shape> shapes;
};

/// <summary>
/// Load a model from an .obj file.
/// </summary>
///
/// <param name="filename">The filename of the model.</param>
RawModelData loadModelData(const std::string& filename, bool opposite_winding = false);

class ModelData {
    friend class Renderer;

public:
    /// <summary>
    /// Setup a model on the GPU.
    /// </summary>
    ///
    /// <param name="data">The model data.</param>
    /// <param name="renderer">The renderer to obtain shader information from.</param>
    ModelData(const RawModelData& data, const Renderer* renderer);

    /// <summary>
    /// Model destructor, ensures that all the buffers generated by the model are cleared.
    /// </summary>
    ~ModelData();

    /// <summary>
    /// Unify all the shapes into a single shape, discarding any extra material and texture information
    /// </summary>
    void unify();

    /// <summary>
    /// Reduces shapes as much as it can without losing any texture data.
    /// </summary>
    void reduce();


private:
    GLuint vao;
    GLuint buffers[4];
    struct Shape {
        Material material;
        GLuint textureId;
        unsigned int elementOffset;
        unsigned int numElements;
    };
    std::vector<Shape> shapes;

    BoundingBox boundingBox;
};
