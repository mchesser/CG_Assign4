#include "Renderer.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLMUtil.hpp"

Renderer::Renderer(GLuint program, glm::mat4 proj, Camera* camera)
    : proj(proj), activeCamera(camera), program(program) {

    shader.in_coord = glGetAttribLocation(program, "v_coord");
    shader.in_normal = glGetAttribLocation(program, "v_normal");
    shader.in_texcoord = glGetAttribLocation(program, "v_texcoord");

    shader.uniform_mv = glGetUniformLocation(program, "mv");
    shader.uniform_proj = glGetUniformLocation(program, "proj");
    shader.uniform_normalMatrix = glGetUniformLocation(program, "normalMatrix");

    shader.uniform_materialAmbient = glGetUniformLocation(program, "material.ambient");
    shader.uniform_materialDiffuse = glGetUniformLocation(program, "material.diffuse");
    shader.uniform_materialSpecular = glGetUniformLocation(program, "material.specular");
    shader.uniform_materialShine = glGetUniformLocation(program, "material.shine");
    shader.uniform_materialOpacity = glGetUniformLocation(program, "material.opacity");
    shader.uniform_lightPosition = glGetUniformLocation(program, "lightPosition");
}

void Renderer::drawModel(const ModelData* model, glm::mat4 transformation) const {
    const glm::mat4 view = activeCamera->view();
    const glm::mat4 mv = view * transformation;
    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mv)));

    glUniformMatrix4fv(shader.uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(shader.uniform_proj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix3fv(shader.uniform_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glUniform3fv(shader.uniform_lightPosition, 1, glm::value_ptr(glm::vec3(view * glm::vec4(10, 100, 0, 0.0))));

    glActiveTexture(GL_TEXTURE0);

    for (size_t i = 0; i < model->shapes.size(); ++i) {
        Material mat = model->shapes[i].material;
        glUniform3fv(shader.uniform_materialAmbient, 1, glm::value_ptr(mat.ambient));
        glUniform3fv(shader.uniform_materialDiffuse, 1, glm::value_ptr(mat.diffuse));
        glUniform3fv(shader.uniform_materialSpecular, 1, glm::value_ptr(mat.specular));
        glUniform1f(shader.uniform_materialShine, mat.shininess);

        glBindTexture(GL_TEXTURE_2D, model->shapes[i].textureId);
        glUniform1i(shader.uniform_texture, /*GL_TEXTURE*/0);

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