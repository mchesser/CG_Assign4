//! Class for managing skybox data
#pragma once
#include "GLHeaders.hpp"
#include "Renderer.hpp"
#include <vector>
#include <string>
class Renderer;


class Skybox {
    friend class Renderer;

public:
    /// <summary>
    /// Setup a skybox on the GPU
    /// </summary>
    ///
    /// <param name="renderer">The Renderer to obtain shader information from</param>
    /// <param name="front_filename">The filename of the image corresponding to the front of the skybox</param>
    /// <param name="back_filename">The filename of the image corresponding to the back of the skybox</param>
    /// <param name="left_filename">The filename of the image corresponding to the left of the skybox</param>
    /// <param name="right_filename">The filename of the image corresponding to the right of the skybox</param>
    /// <param name="top_filename">The filename of the image corresponding to the top of the skybox</param>
    /// <param name="bottom_filename">The filename of the image corresponding to the bottom of the skybox</param>
    Skybox(const Renderer* renderer,
        std::string front_filename,
        std::string back_filename,
        std::string left_filename,
        std::string right_filename,
        std::string top_filename,
        std::string bottom_filename,
        std::vector<std::string> night_files,
        std::vector<std::string> sunset_files);

private:
    struct Wall {
        GLuint vao;
        GLuint buffers[3];
        GLuint day_textureId;
        GLuint sunset_textureId;
        GLuint night_textureId;
        unsigned int num_elements;
    } walls[6];
};