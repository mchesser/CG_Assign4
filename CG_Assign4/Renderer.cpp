#include "Renderer.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLMUtil.hpp"

Renderer::Renderer(GLuint program, glm::mat4 proj, Camera* camera)
    : proj(proj), activeCamera(camera), program(program) {

    // Configure shaders
    shader.in_coord = glGetAttribLocation(program, "v_coord");
    shader.in_normal = glGetAttribLocation(program, "v_normal");
    shader.in_texcoord = glGetAttribLocation(program, "v_texcoord");

    shader.uniform_mv = glGetUniformLocation(program, "mv");
    shader.uniform_proj = glGetUniformLocation(program, "proj");
    shader.uniform_depthBiasMVP = glGetUniformLocation(program, "depthBiasMVP");
    shader.uniform_normalMatrix = glGetUniformLocation(program, "normalMatrix");

    shader.uniform_materialAmbient = glGetUniformLocation(program, "material.ambient");
    shader.uniform_materialDiffuse = glGetUniformLocation(program, "material.diffuse");
    shader.uniform_materialSpecular = glGetUniformLocation(program, "material.specular");
    shader.uniform_materialShine = glGetUniformLocation(program, "material.shine");
    shader.uniform_materialOpacity = glGetUniformLocation(program, "material.opacity");
    shader.uniform_lightPosition = glGetUniformLocation(program, "lightPosition");
    shader.uniform_texture = glGetUniformLocation(program, "texture");
    shader.uniform_shadowMap = glGetUniformLocation(program, "shadowMap");
}

void Renderer::drawModel(const ModelData* model, glm::mat4 transformation) const {
    const glm::mat4 view = activeCamera->view();
    const glm::mat4 mv = view * transformation;
    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mv)));

    glUniformMatrix4fv(shader.uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(shader.uniform_proj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix3fv(shader.uniform_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    // Bind the shadow map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapId);
    glUniform1i(shader.uniform_shadowMap, /*GL_TEXTURE*/0);

    glUniform3fv(shader.uniform_lightPosition, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lightPos, 1.0f))));
    
    // Setup shadow map transformation
    const glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0, 1, 0));
    const glm::mat4 depthMVP = glm::ortho<float>(-100, 100, -100, 100, -100, 200) * lightView * transformation;
    const glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
    glUniformMatrix4fv(shader.uniform_depthBiasMVP, 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

    glActiveTexture(GL_TEXTURE1);
    for (size_t i = 0; i < model->shapes.size(); ++i) {
        Material mat = model->shapes[i].material;
        glUniform3fv(shader.uniform_materialAmbient, 1, glm::value_ptr(mat.ambient));
        glUniform3fv(shader.uniform_materialDiffuse, 1, glm::value_ptr(mat.diffuse));
        glUniform3fv(shader.uniform_materialSpecular, 1, glm::value_ptr(mat.specular));
        glUniform1f(shader.uniform_materialShine, mat.shininess);

        glBindTexture(GL_TEXTURE_2D, model->shapes[i].textureId);
        glUniform1i(shader.uniform_texture, /*GL_TEXTURE*/1);

        glBindVertexArray(model->shapes[i].vao);
        glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, NULL);
    }
}

void Renderer::drawModel(const ModelData* model, glm::vec3 position, glm::vec3 scale,
    glm::vec3 rotation) const {
    drawModel(model,
        glm::translate(glm::mat4(1.0f), position) *
        rotate3D(rotation) *
        glm::scale(glm::mat4(1.0f), scale)
        );
}