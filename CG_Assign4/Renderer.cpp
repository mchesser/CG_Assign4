#include "Renderer.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLMUtil.hpp"

#define TAU (6.283185307179586f)
#define DEG2RAD(x) ((x) / 360.0f * TAU)

Renderer::Renderer(GLsizei screenWidth, GLsizei screenHeight, const Camera* camera, const Sun* sun, GLuint modelProgram,
    GLuint shadowMapProgram) : screenWidth(screenWidth), screenHeight(screenHeight),
    activeCamera(camera), sun(sun), modelProgram(modelProgram), shadowMapProgram(shadowMapProgram) {
    
    // Configure shaders
    shader.in_coord = glGetAttribLocation(modelProgram, "v_coord");
    shader.in_normal = glGetAttribLocation(modelProgram, "v_normal");
    shader.in_texcoord = glGetAttribLocation(modelProgram, "v_texcoord");

    shader.uniform_mv = glGetUniformLocation(modelProgram, "mv");
    shader.uniform_proj = glGetUniformLocation(modelProgram, "proj");
    shader.uniform_depthBiasMVP = glGetUniformLocation(modelProgram, "depthBiasMVP");
    shader.uniform_normalMatrix = glGetUniformLocation(modelProgram, "normalMatrix");

    shader.uniform_materialAmbient = glGetUniformLocation(modelProgram, "material.ambient");
    shader.uniform_materialDiffuse = glGetUniformLocation(modelProgram, "material.diffuse");
    shader.uniform_materialSpecular = glGetUniformLocation(modelProgram, "material.specular");
    shader.uniform_materialShine = glGetUniformLocation(modelProgram, "material.shine");
    shader.uniform_materialOpacity = glGetUniformLocation(modelProgram, "material.opacity");
    
    shader.uniform_sunPos = glGetUniformLocation(modelProgram, "sunPos");
    shader.uniform_sunAmbient = glGetUniformLocation(modelProgram, "sunAmbient");
    shader.uniform_sunDiffuse = glGetUniformLocation(modelProgram, "sunDiffuse");

    shader.uniform_modelTexture = glGetUniformLocation(modelProgram, "modelTexture");
    shader.uniform_shadowMap = glGetUniformLocation(modelProgram, "shadowMap");
    shader.uniform_depthMVP = glGetUniformLocation(shadowMapProgram, "depthMVP");

    // Configure shadow map buffers
    glGenFramebuffers(1, &shadowMapFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);

    // Setup depth texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    // Setup a 1024x1024 texture image with no data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 4 * 1024, 4 * 1024, 0, GL_DEPTH_COMPONENT,
        GL_FLOAT, NULL);

    // Setup sampling settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // Attach the texture object to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);

    // Don't draw to the colour buffer
    glDrawBuffer(GL_NONE);

    // Check that the framebuffer is working.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }
}

Renderer::~Renderer() {
    // Free the framebuffer and texture
    glDeleteFramebuffers(1, &shadowMapFramebuffer);
    glDeleteTextures(1, &shadowMapTexture);
}

void Renderer::resize(GLsizei width, GLsizei height) {
    screenWidth = width;
    screenHeight = height;
}

void Renderer::drawModel(const ModelData* model, glm::mat4 transformation) {
	RenderData data = { model, transformation };
	renderData.push_back(data);
}

void Renderer::drawModel(const ModelData* model, glm::vec3 position, glm::vec3 scale,
    glm::vec3 rotation) {
    const glm::mat4 transformation =
        glm::translate(glm::mat4(1.0f), position) *
        rotate3D(rotation) *
        glm::scale(glm::mat4(1.0f), scale);
    drawModel(model, transformation);
}

void Renderer::renderScene() const {
    const glm::mat4 cameraView = activeCamera->view();
    const glm::mat4 sunViewProj = sun->viewProjection(activeCamera->getPosition());

    //
    // Render shadowmap
    //
    glUseProgram(shadowMapProgram);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 4 * 1024, 4 * 1024);
    for (size_t i = 0; i < renderData.size(); ++i) {
        const glm::mat4 depthMVP = sunViewProj * renderData[i].transformation;
        glUniformMatrix4fv(shader.uniform_depthMVP, 1, GL_FALSE, glm::value_ptr(depthMVP));

        const ModelData* model = renderData[i].model;
        for (size_t i = 0; i < model->shapes.size(); ++i) {
            glBindVertexArray(model->shapes[i].vao);
            glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, NULL);
        }
    }
    
    //
    // Render models
    //
    glUseProgram(modelProgram);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shadowmap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(shader.uniform_shadowMap, /*GL_TEXTURE*/0);

    // Change the active texture (used later)
    glActiveTexture(GL_TEXTURE1);

    glUniform3fv(shader.uniform_sunPos, 1, glm::value_ptr(glm::vec3(cameraView * glm::vec4(sun->position(), 1.0f))));
    glUniform3fv(shader.uniform_sunAmbient, 1, glm::value_ptr(sun->ambient()));
    glUniform3fv(shader.uniform_sunDiffuse, 1, glm::value_ptr(sun->diffuse()));

    const glm::mat4 cameraProj = glm::perspective(DEG2RAD(60.0f), aspectRatio(), 0.1f, 200.0f);
    glUniformMatrix4fv(shader.uniform_proj, 1, GL_FALSE, glm::value_ptr(cameraProj));

    for (size_t i = 0; i < renderData.size(); ++i) {
        // Calculate shadowmap transformations
        const glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
         );
        const glm::mat4 depthMVP = sunViewProj * renderData[i].transformation;
        const glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
        glUniformMatrix4fv(shader.uniform_depthBiasMVP, 1, GL_FALSE, glm::value_ptr(depthBiasMVP));
        
        // Calculate model transformations
        const glm::mat4 mv = cameraView * renderData[i].transformation;
        glUniformMatrix4fv(shader.uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mv)));
        glUniformMatrix3fv(shader.uniform_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        // Render the model
        const ModelData* model = renderData[i].model;
        for (size_t i = 0; i < model->shapes.size(); ++i) {
            Material mat = model->shapes[i].material;
            glUniform3fv(shader.uniform_materialAmbient, 1, glm::value_ptr(mat.ambient));
            glUniform3fv(shader.uniform_materialDiffuse, 1, glm::value_ptr(mat.diffuse));
            glUniform3fv(shader.uniform_materialSpecular, 1, glm::value_ptr(mat.specular));
            glUniform1f(shader.uniform_materialShine, mat.shininess);

            glBindTexture(GL_TEXTURE_2D, model->shapes[i].textureId);
            glUniform1i(shader.uniform_modelTexture, /*GL_TEXTURE*/1);

            glBindVertexArray(model->shapes[i].vao);
            glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, NULL);
        }
    }
}

void Renderer::clear() {
    renderData.clear();
}

float Renderer::aspectRatio() const {
    return static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
}
