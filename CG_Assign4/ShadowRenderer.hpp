//! A class for rendering a shadow map
#pragma once
#include "ModelData.hpp"
#include "Camera.hpp"
#include "glm/mat4x4.hpp"


class ShadowRenderer {
public:
    ShadowRenderer(GLuint program);
    ~ShadowRenderer();

    /// <summary>
    /// Draw a model in the shadow map after applying a specific transformation.
    /// </summary>
    ///
    /// <param name="model">The model to draw.</param>
    /// <param name="transformation">The transformation to apply to the model</param>
    void drawModel(const ModelData* model, glm::mat4 transformation) const;

    void drawModel(const ModelData* model,
        glm::vec3 position = glm::vec3(0),
        glm::vec3 scale = glm::vec3(1),
        glm::vec3 rotation = glm::vec3(0)) const;

    GLuint depthTexture;
    GLuint program;
    GLuint frameBuffer;
    glm::vec3 lightPos;
    glm::mat4 proj;

    Camera* activeCamera;

private:


    struct ShaderInfo {
        GLint in_coord;
        GLint uniform_depthMVP;
    } shader;
};

