//! A class that manages: shaders, lights and transformation matrices
#pragma once

#include "GLHeaders.hpp"
#include "Camera.hpp"
#include "Sun.hpp"
#include "ModelData.hpp"
#include "glm/mat4x4.hpp"

class ModelData;

class Renderer {
public:
    /// <summary>
    /// Create a new renderer.
    /// </summary>
    ///
    /// <param name="screenWidth">The width of the renderer's viewport.</param>
    /// <param name="screenHeight">The height of the renderer's viewport.</param>
    /// <param name="renderDistance">The distance to draw.</param>
    /// <param name="camera">The renderer's active camera.</param>
    /// <param name="sun">The sun used for lighting and shadows.</param>
    /// <param name="modelProgram">The id of the model shader program.</param>
    /// <param name="shadowMapProgram">The id of the shadowMap shader program.</param>
    Renderer(GLsizei screenWidth, GLsizei screenHeight, float renderDistance, const Camera* camera, const Sun* sun,
        GLuint modelProgram, GLuint shadowMapProgram);

    /// <summary>
    /// Renderer destructor, frees the buffers and textures allocated by the renderer.
    /// </summary>
    ~Renderer();

    /// <summary>
    /// Resizes the renderer's viewport
    /// <summary>
    ///
    /// <param name="width">The new width.</param>
    /// <param name="height">The new height.</param>
    void resize(GLsizei width, GLsizei height);

    /// <summary>
    /// Draw a model with a specific transformation
    /// </summary>
    ///
    /// <param name="model">The model to draw.</param>
    /// <param name="transformation">The transformation to apply to the model</param>
    void drawModel(const ModelData* model, glm::mat4 transformation);

    /// <summary>
    /// Draw a model with transformation properties.
    /// </summary>
    ///
    /// <param name="model">The model to draw.</param>
    /// <param name="position">The position of the model in the world.</param>
    /// <param name="scale">The scale of the model.</param>
    /// <param name="rotation">The rotation of the model.</param>
    void drawModel(const ModelData* model,
        glm::vec3 position = glm::vec3(0),
        glm::vec3 scale = glm::vec3(1),
        glm::vec3 rotation = glm::vec3(0));

    /// <summary>
    /// Renders the scene to the screen.
    /// </summary>
    void renderScene() const;
    
    /// <summary>
    /// Clears the current scene in the renderer.
    /// </summary>
    void clear();

    /// <summary>
    /// Checks for collision at position.
    /// </summary>
    bool checkCollision(glm::vec3 position);
   
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

        GLint uniform_sunPos;
        GLint uniform_sunAmbient;
        GLint uniform_sunDiffuse;

        GLint uniform_modelTexture;
        GLint uniform_shadowMap;
        GLint uniform_depthMVP;

        GLint uniform_renderDistance;
    } shader;

    GLsizei screenWidth;
    GLsizei screenHeight;
    float renderDistance;

    /// <summary>
    /// The camera to draw from.
    /// </summary>
    const Camera* activeCamera;

    const Sun* sun;
private:
    GLuint modelProgram;
    GLuint shadowMapProgram;
    
    GLuint shadowMapFramebuffer;
    GLuint shadowMapTexture;

    glm::vec3 lightPos;

    struct RenderData {
        const ModelData* model;
        glm::mat4 transformation;
    };

    std::vector<RenderData> renderData;

    /// <summary>
    /// Computes the current aspect ratio of the renderer's screen.
    /// </summary>
    float aspectRatio() const;
};
