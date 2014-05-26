//! A class that manages: shaders, lights and transformation matrices
#pragma once

#include "GLHeaders.hpp"
#include "Camera.hpp"
#include "ModelData.hpp"
#include "glm/mat4x4.hpp"

class ModelData;

class Renderer {
public:
    /// <summary>
    /// Create a new renderer.
    /// </summary>
    ///
    /// <param name="program">The id of the gl shader program.</param>
    /// <param name="proj">The perspective projection to apply.</param>
    /// <param name="camera">The initial camera.</param>
    Renderer(GLuint program, glm::mat4 proj, Camera* camera);

    /// <summary>
    /// Draw a model after applying a specific transformation
    /// </summary>
    ///
    /// <param name="model">The model to draw.</param>
    /// <param name="transformation">The transformation to apply to the model</param>
    void drawModel(const ModelData* model, glm::mat4 transformation) const;

    /// <summary>
    /// Draw a model after applying different transformations.
    /// </summary>
    ///
    /// <param name="model">The model to draw.</param>
    /// <param name="position">The position of the model in the world.</param>
    /// <param name="scale">The scale of the model.</param>
    /// <param name="rotation">The rotation of the model.</param>
    void drawModel(const ModelData* model,
        glm::vec3 position = glm::vec3(0),
        glm::vec3 scale = glm::vec3(1),
        glm::vec3 rotation = glm::vec3(0)) const;

    /// <summary>
    /// The perspective projection.
    /// </summary>
    glm::mat4 proj;

    /// <summary>
    /// The camera to draw from.
    /// </summary>
    Camera* activeCamera;

    struct ShaderInfo {
        GLint in_coord;
        GLint in_normal;
        GLint in_texcoord;

        GLint uniform_mv;
        GLint uniform_proj;
        GLint uniform_depthBiasMVP;
        GLint uniform_normalMatrix;

        GLint uniform_materialAmbient;
        GLint uniform_materialDiffuse;
        GLint uniform_materialSpecular;
        GLint uniform_materialShine;
        GLint uniform_materialOpacity;

        GLint uniform_lightPosition;

        GLint uniform_texture;
        GLint uniform_shadowMap;
    } shader;

    GLuint shadowMapId;
    GLuint program;

    glm::vec3 lightPos;
private:
};