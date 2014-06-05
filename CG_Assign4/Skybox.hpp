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
    /// <param name="day_files">Contains filenames for the 6 files corresponding to the skybox at daytime, in the following order: front, back, left, right, top, bottom</param>
    /// <param name="night_files">Contains filenames for the 6 files corresponding to the skybox at nighttime, in the following order: front, back, left, right, top, bottom</param>
    /// <param name="sunset_files">Contains filenames for the 6 files corresponding to the skybox at sunset, in the following order: front, back, left, right, top, bottom</param>
    Skybox(const Renderer* renderer,
        std::vector<std::string> day_files,
        std::vector<std::string> night_files,
        std::vector<std::string> sunset_files);

    ~Skybox();

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