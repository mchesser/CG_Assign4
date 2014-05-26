#include "ShadowRenderer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLMUtil.hpp"

#define TAU (6.283185307179586f)
#define DEG2RAD(x) ((x) / 360.0f * TAU)

ShadowRenderer::ShadowRenderer(GLuint program) : program(program) {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Setup depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
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
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // Don't draw to the colour buffer
    glDrawBuffer(GL_NONE);

    // Check that the framebuffer is working.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }

    // Get shader variables
    shader.in_coord = glGetAttribLocation(program, "v_coord");
    shader.uniform_depthMVP = glGetUniformLocation(program, "depthMVP");

    // Set up projection matrix
    proj = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
}

ShadowRenderer::~ShadowRenderer() {
    // Free the framebuffer and texture
   glDeleteFramebuffers(1, &frameBuffer);
   glDeleteTextures(1, &depthTexture);
}

void ShadowRenderer::drawModel(const ModelData* model, glm::mat4 transformation) const {
    const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0, 1, 0));
    const glm::mat4 depthMVP = proj * lightView * transformation;

    glUniformMatrix4fv(shader.uniform_depthMVP, 1, GL_FALSE, glm::value_ptr(depthMVP));
    for (size_t i = 0; i < model->shapes.size(); ++i) {
        glBindVertexArray(model->shapes[i].vao);
        glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, NULL);
    }
}

void ShadowRenderer::drawModel(const ModelData* model, glm::vec3 position, glm::vec3 scale,
    glm::vec3 rotation) const {
    drawModel(model,
        glm::translate(glm::mat4(1.0f), position) *
        rotate3D(rotation) *
        glm::scale(glm::mat4(1.0f), scale)
        );
}