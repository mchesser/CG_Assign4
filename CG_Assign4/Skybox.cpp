#include "Skybox.hpp"
#include "SOIL2/SOIL2.h"
#include <iostream>
#include <vector>
#include <string>

//Define size of skybox
#define SKYSIZE (50.0f)


// Define Indicees used for internal Wall struct
#define WALL_UP (4)
#define WALL_DOWN (5)
#define WALL_LEFT (2)
#define WALL_RIGHT (3)
#define WALL_FRONT (0)
#define WALL_BACK (1)


Skybox::Skybox(const Renderer* renderer, std::vector<std::string> day_files, std::vector<std::string> night_files, std::vector<std::string> sunset_files) {


    //
    // Initialize wall vertices
    //

    GLfloat vertices[6][12];

    // Initialize Wall Up
    vertices[WALL_UP][0] = SKYSIZE; vertices[WALL_UP][1] = SKYSIZE; vertices[WALL_UP][2] = -SKYSIZE;
    vertices[WALL_UP][3] = SKYSIZE; vertices[WALL_UP][4] = SKYSIZE; vertices[WALL_UP][5] = SKYSIZE;
    vertices[WALL_UP][6] = -SKYSIZE; vertices[WALL_UP][7] = SKYSIZE; vertices[WALL_UP][8] = SKYSIZE;
    vertices[WALL_UP][9] = -SKYSIZE; vertices[WALL_UP][10] = SKYSIZE;  vertices[WALL_UP][11] = -SKYSIZE;

    // Initialize Wall Down
    vertices[WALL_DOWN][0] = SKYSIZE; vertices[WALL_DOWN][1] = -SKYSIZE; vertices[WALL_DOWN][2] = SKYSIZE;
    vertices[WALL_DOWN][3] = SKYSIZE; vertices[WALL_DOWN][4] = -SKYSIZE; vertices[WALL_DOWN][5] = -SKYSIZE;
    vertices[WALL_DOWN][6] = -SKYSIZE; vertices[WALL_DOWN][7] = -SKYSIZE; vertices[WALL_DOWN][8] = -SKYSIZE;
    vertices[WALL_DOWN][9] = -SKYSIZE; vertices[WALL_DOWN][10] = -SKYSIZE;  vertices[WALL_DOWN][11] = SKYSIZE;

    // Initialize Wall Left
    vertices[WALL_LEFT][0] = -SKYSIZE; vertices[WALL_LEFT][1] = SKYSIZE; vertices[WALL_LEFT][2] = SKYSIZE;
    vertices[WALL_LEFT][3] = -SKYSIZE; vertices[WALL_LEFT][4] = -SKYSIZE; vertices[WALL_LEFT][5] = SKYSIZE;
    vertices[WALL_LEFT][6] = -SKYSIZE; vertices[WALL_LEFT][7] = -SKYSIZE; vertices[WALL_LEFT][8] = -SKYSIZE;
    vertices[WALL_LEFT][9] = -SKYSIZE; vertices[WALL_LEFT][10] = SKYSIZE;  vertices[WALL_LEFT][11] = -SKYSIZE;

    // Initialize Wall Right
    vertices[WALL_RIGHT][0] = SKYSIZE; vertices[WALL_RIGHT][1] = SKYSIZE; vertices[WALL_RIGHT][2] = -SKYSIZE;
    vertices[WALL_RIGHT][3] = SKYSIZE; vertices[WALL_RIGHT][4] = -SKYSIZE; vertices[WALL_RIGHT][5] = -SKYSIZE;
    vertices[WALL_RIGHT][6] = SKYSIZE; vertices[WALL_RIGHT][7] = -SKYSIZE; vertices[WALL_RIGHT][8] = SKYSIZE;
    vertices[WALL_RIGHT][9] = SKYSIZE; vertices[WALL_RIGHT][10] = SKYSIZE;  vertices[WALL_RIGHT][11] = SKYSIZE;

    // Initialize wall Front
    vertices[WALL_FRONT][0] = SKYSIZE; vertices[WALL_FRONT][1] = SKYSIZE; vertices[WALL_FRONT][2] = SKYSIZE;
    vertices[WALL_FRONT][3] = SKYSIZE; vertices[WALL_FRONT][4] = -SKYSIZE; vertices[WALL_FRONT][5] = SKYSIZE;
    vertices[WALL_FRONT][6] = -SKYSIZE; vertices[WALL_FRONT][7] = -SKYSIZE; vertices[WALL_FRONT][8] = SKYSIZE;
    vertices[WALL_FRONT][9] = -SKYSIZE; vertices[WALL_FRONT][10] = SKYSIZE;  vertices[WALL_FRONT][11] = SKYSIZE;

    // Initialize wall Back
    vertices[WALL_BACK][0] = -SKYSIZE; vertices[WALL_BACK][1] = SKYSIZE; vertices[WALL_BACK][2] = -SKYSIZE;
    vertices[WALL_BACK][3] = -SKYSIZE; vertices[WALL_BACK][4] = -SKYSIZE; vertices[WALL_BACK][5] = -SKYSIZE;
    vertices[WALL_BACK][6] = SKYSIZE; vertices[WALL_BACK][7] = -SKYSIZE; vertices[WALL_BACK][8] = -SKYSIZE;
    vertices[WALL_BACK][9] = SKYSIZE; vertices[WALL_BACK][10] = SKYSIZE;  vertices[WALL_BACK][11] = -SKYSIZE;


    //
    // Initialize wall texcoords
    //

    GLfloat texcoords[] = {
        0, 1, 0, 0, 1, 0, 1, 1
    };

    //
    // Initialize wall indices
    //

    unsigned int indices[] = {
        2, 1, 0, 0, 3, 2
    };


    //
    // Load data into GPU buffers and initialize appropriate GLSL variables
    //
    for (int i = 0; i < 6; i++) {
        glGenVertexArrays(1, &walls[i].vao);
        glBindVertexArray(walls[i].vao);

        glGenBuffers(3, walls[i].buffers);

        glBindBuffer(GL_ARRAY_BUFFER, walls[i].buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_STATIC_DRAW);
        glEnableVertexAttribArray(renderer->shader.in_sb_coord);
        glVertexAttribPointer(renderer->shader.in_sb_coord, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        //Load texture coordinates into buffer
        glBindBuffer(GL_ARRAY_BUFFER, walls[i].buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(renderer->shader.in_sb_texcoord);
        glVertexAttribPointer(renderer->shader.in_sb_texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        //Load textures using SOIL
        walls[i].day_textureId = SOIL_load_OGL_texture(day_files[i].c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

        walls[i].night_textureId = SOIL_load_OGL_texture(night_files[i].c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

        walls[i].sunset_textureId = SOIL_load_OGL_texture(sunset_files[i].c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

        // Load indices into buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, walls[i].buffers[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        walls[i].num_elements = 6;

        glBindVertexArray(0);
    }
}


Skybox::~Skybox() {
    for (int i = 0; i < 6; i++) {
        glDeleteBuffers(3, walls[i].buffers);
        glDeleteTextures(1, &walls[i].day_textureId);
        glDeleteTextures(1, &walls[i].sunset_textureId);
        glDeleteTextures(1, &walls[i].night_textureId);
        glDeleteVertexArrays(1, &walls[i].vao);
    }
}