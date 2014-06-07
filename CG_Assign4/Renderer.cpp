#include "Renderer.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLMUtil.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

#define TAU (6.283185307179586f)
#define FOV 45.0f
#define DEG2RAD(x) ((x) / 360.0f * TAU)

Renderer::Renderer(GLsizei screenWidth, GLsizei screenHeight, float renderDistance, const Camera* camera, const Sun* sun,
    GLuint modelProgram, GLuint shadowMapProgram, GLuint skyboxProgram) : screenWidth(screenWidth),  
    screenHeight(screenHeight), renderDistance(renderDistance), activeCamera(camera), sun(sun), 
    modelProgram(modelProgram), shadowMapProgram(shadowMapProgram), skyboxProgram(skyboxProgram) {
    
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
    shader.uniform_isDay = glGetUniformLocation(modelProgram, "isDay");

    shader.uniform_modelTexture = glGetUniformLocation(modelProgram, "modelTexture");
    shader.uniform_shadowMap = glGetUniformLocation(modelProgram, "shadowMap");
    shader.uniform_depthMVP = glGetUniformLocation(shadowMapProgram, "depthMVP");

    shader.uniform_fogColor = glGetUniformLocation(modelProgram, "fogColor");

    shader.uniform_renderDistance = glGetUniformLocation(modelProgram, "renderDistance");

    shader.in_sb_coord = glGetAttribLocation(skyboxProgram, "v_coord");
    shader.in_sb_texcoord = glGetAttribLocation(skyboxProgram, "texcoord");

    shader.uniform_sb_rotate = glGetUniformLocation(skyboxProgram, "rotate");
    shader.uniform_sb_proj = glGetUniformLocation(skyboxProgram, "proj");
    shader.uniform_sb_day_texture = glGetUniformLocation(skyboxProgram, "day_texture");
    shader.uniform_sb_night_texture = glGetUniformLocation(skyboxProgram, "night_texture");
    shader.uniform_sb_sunset_texture = glGetUniformLocation(skyboxProgram, "sunset_texture");
    shader.uniform_sb_sun_pos = glGetUniformLocation(skyboxProgram, "sun_position");

    // Configure lights uniform
    shader.uniform_numLights = glGetUniformLocation(modelProgram, "numLights");
    shader.uniform_lampLight.direction = glGetUniformLocation(modelProgram, "lampLight.direction");
    shader.uniform_lampLight.maxAngle = glGetUniformLocation(modelProgram, "lampLight.maxAngle");
    shader.uniform_lampLight.ambient = glGetUniformLocation(modelProgram, "lampLight.ambient");
    shader.uniform_lampLight.diffuse = glGetUniformLocation(modelProgram, "lampLight.diffuse");
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        std::ostringstream light_ind;
        light_ind << i;
        const std::string shaderName = "lightPositions[" + light_ind.str() + "]";
        shader.uniform_lightPositions[i]= glGetUniformLocation(modelProgram, (shaderName).c_str());
    }
    lampLight.direction = glm::vec3(0, -1, 0);
    lampLight.maxAngle = 1.1f;
    lampLight.ambient = glm::vec3(0.0);
    lampLight.diffuse = glm::vec3(1.0, 0.8, 0.6);

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

    // Initialize skybox to empty
    active_skybox = NULL;
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

void Renderer::addLight(glm::vec3 position) {
    lights.push_back(position);
}

struct LightSorter {
    glm::vec3 origin;
    glm::vec3 viewDirection;

    bool operator()(const glm::vec3& l1, const glm::vec3& l2) const {
        return glm::length(l1 - origin) < glm::length(l2 - origin);
    }

    bool inFOV(const glm::vec3& pt) const {
        return glm::abs(glm::acos(glm::dot(viewDirection, glm::normalize(pt - origin)))) < DEG2RAD(FOV);
    }
};

bool inFOV(glm::vec3 pt, glm::vec3 origin, glm::vec3 viewDirection) {
    if (glm::length(pt - origin) < 5.0f) {
        return true;
    }

    return glm::abs(glm::acos(glm::dot(viewDirection, glm::normalize(pt - origin)))) < DEG2RAD(FOV);
}

void Renderer::renderScene() {
    const glm::mat4 cameraView = activeCamera->view();
    const glm::mat4 sunViewProj = sun->viewProjection(activeCamera->getPosition());
    const glm::vec3 sunPosition = sun->position();

    //
    // Render shadowmap
    //

    // Render the shadow map only if the sun position is above the horizon
    if (sunPosition.y > 0) {
        glUseProgram(shadowMapProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 4 * 1024, 4 * 1024);
        for (size_t i = 0; i < renderData.size(); ++i) {
            const glm::mat4 depthMVP = sunViewProj * renderData[i].transformation;
            glUniformMatrix4fv(shader.uniform_depthMVP, 1, GL_FALSE, glm::value_ptr(depthMVP));

            const ModelData* model = renderData[i].model;
            glBindVertexArray(model->vao);
            for (size_t i = 0; i < model->shapes.size(); ++i) {
                glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, (GLvoid*)model->shapes[i].elementOffset);
            }
        }
    }
    
    //
    // Set background color and fog color
    //
    const glm::vec3 day = glm::vec3(0.7f, 0.8f, 1.0f);
    const glm::vec3 sunset = glm::vec3(1.0f, 0.76f, 0.43f);
    const glm::vec3 night = glm::vec3(0.0f, 0.0f, 0.03f);
    const glm::vec4 daytime_fog = glm::vec4(1.0, 1.0, 1.0, 1.0);
    const glm::vec4 nighttime_fog = glm::vec4(0.0, 0.0, 0.0, 1.0);

    glm::vec4 fogColor;
    glm::vec3 clearColor;

    // Night
    if (sunPosition.y < 0) {
        fogColor = nighttime_fog;
        clearColor = night;
    } 
    // Night -> Sunrise / sunset
    else if (sunPosition.y < 100) {
        float t = 1.0f - (sunPosition.y / 100.0f);

        fogColor = (t / 2.0f) * daytime_fog;
        clearColor = t * night + (1.0f - t) * sunset;
    }
    // Sunrise / sunset -> Day
    else if (sunPosition.y < 200) {
        float t = 1.0f - ((sunPosition.y - 100.0f) / 100.0f);

        fogColor = (t / 2.0f + 0.5f) * daytime_fog;
        clearColor = t * sunset + (1.0f - t) * day;
    }
    // Day
    else {
        fogColor = daytime_fog;
        clearColor = day;
    }
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);

    //
    // Render active skybox
    //
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw if there is an active skybox
    if (active_skybox != NULL) {
        glUseProgram(skyboxProgram);

        // set appropriate projection for skybox
        glm::mat4 rotate = cameraView * glm::translate(glm::mat4(1), activeCamera->getPosition());
        glm::mat4 proj = glm::perspective(DEG2RAD(FOV), aspectRatio(), 0.1f, 100.0f);
        glm::vec3 sun_position = sun->position();
        glUniformMatrix4fv(shader.uniform_sb_rotate, 1, GL_FALSE, glm::value_ptr(rotate));
        glUniformMatrix4fv(shader.uniform_sb_proj, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(shader.uniform_sb_sun_pos, 1, glm::value_ptr(sun_position));

        // Draw the 6 walls of the skybox
        for (int i = 0; i < 6; i++) {
            glBindVertexArray(active_skybox->walls[i].vao);

            glActiveTexture(GL_TEXTURE0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, active_skybox->walls[i].day_textureId);
            glUniform1i(shader.uniform_sb_day_texture, 0);

            glActiveTexture(GL_TEXTURE1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, active_skybox->walls[i].night_textureId);
            glUniform1i(shader.uniform_sb_night_texture, 1);

            glActiveTexture(GL_TEXTURE2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, active_skybox->walls[i].sunset_textureId);
            glUniform1i(shader.uniform_sb_sunset_texture, 2);
            
            glDrawElements(GL_TRIANGLES, active_skybox->walls[i].num_elements, GL_UNSIGNED_INT, NULL);

            glBindVertexArray(0);
        }

        glUseProgram(0);
    }
    
    //
    // Render models
    //
    glUseProgram(modelProgram);
    glUniform1f(shader.uniform_renderDistance, renderDistance);

    // Bind shadowmap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(shader.uniform_shadowMap, /*GL_TEXTURE*/0);

    // Change the active texture (used later)
    glActiveTexture(GL_TEXTURE1);

    // Add lights
    glUniform3fv(shader.uniform_sunPos, 1, glm::value_ptr(glm::vec3(cameraView * glm::vec4(sunPosition, 1.0f))));
    glUniform3fv(shader.uniform_sunAmbient, 1, glm::value_ptr(sun->ambient()));
    glUniform3fv(shader.uniform_sunDiffuse, 1, glm::value_ptr(sun->diffuse()));
    glUniform3fv(shader.uniform_fogColor, 1, glm::value_ptr(fogColor));
    glUniform1i(shader.uniform_isDay, (GLboolean)(sunPosition.y > 0.0f));

    // Sort the lights so that the nearest lights are more likely to be shown
    LightSorter sorter = { activeCamera->getPosition(), activeCamera->getDirection() };
    std::sort(lights.begin(), lights.end(), sorter);

    glUniform3fv(shader.uniform_lampLight.direction, 1, glm::value_ptr(glm::vec3(cameraView * glm::vec4(lampLight.direction, 0.0))));
    glUniform1f(shader.uniform_lampLight.maxAngle, lampLight.maxAngle);
    glUniform3fv(shader.uniform_lampLight.ambient, 1, glm::value_ptr(lampLight.ambient));
    glUniform3fv(shader.uniform_lampLight.diffuse, 1, glm::value_ptr(lampLight.diffuse));

    int shader_i = 0;
    int light_i = 0;
    while (shader_i < MAX_LIGHTS) {
        while (light_i < lights.size() && !inFOV(lights[light_i], activeCamera->getPosition(), activeCamera->getDirection())) {
            light_i += 1;
        }
        if (light_i >= lights.size()) {
            break;
        }

        glUniform3fv(shader.uniform_lightPositions[shader_i], 1, glm::value_ptr(glm::vec3(cameraView * glm::vec4(lights[light_i], 1.0f))));

        light_i += 1;
        shader_i += 1;
    }
    glUniform1i(shader.uniform_numLights, shader_i);

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
        glBindVertexArray(model->vao);
        for (size_t i = 0; i < model->shapes.size(); ++i) {
            Material mat = model->shapes[i].material;
            glUniform3fv(shader.uniform_materialAmbient, 1, glm::value_ptr(mat.ambient));
            glUniform3fv(shader.uniform_materialDiffuse, 1, glm::value_ptr(mat.diffuse));
            glUniform3fv(shader.uniform_materialSpecular, 1, glm::value_ptr(mat.specular));
            glUniform1f(shader.uniform_materialShine, mat.shininess);

            glBindTexture(GL_TEXTURE_2D, model->shapes[i].textureId);
            glUniform1i(shader.uniform_modelTexture, /*GL_TEXTURE*/1);

            glDrawElements(GL_TRIANGLES, model->shapes[i].numElements, GL_UNSIGNED_INT, (GLvoid*)model->shapes[i].elementOffset);
        }
    }
}

bool Renderer::checkCollision(glm::vec3 position) { 
    for (size_t i = 0; i < renderData.size(); i++) {
        // Position of object
        const glm::mat4 m = renderData[i].transformation;

        // put bounding box in position
        glm::vec4 boundingBoxMax = m * glm::vec4(renderData[i].model->boundingBox.maxVertex, 1);
        glm::vec4 boundingBoxMin = m * glm::vec4(renderData[i].model->boundingBox.minVertex, 1); 

        float boundingOffset = 0.3f;
        //Check if within box
        if (boundingBoxMax.x + boundingOffset > position.x 
            && boundingBoxMin.x - boundingOffset < position.x
            && boundingBoxMax.y + boundingOffset > position.y
            && boundingBoxMin.y - boundingOffset < position.y
            && boundingBoxMax.z + boundingOffset > position.z
            && boundingBoxMin.z - boundingOffset < position.z) {
            return true;
        } 
        else if (position.y < 0.0 + boundingOffset) { // Special case for the ground
            return true;
        }
    }
    return false;
}


void Renderer::attachSkybox(Skybox* skybox) {
    active_skybox = skybox;
}

void Renderer::clear() {
    renderData.clear();
    lights.clear();
}

float Renderer::aspectRatio() const {
    return static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
}
