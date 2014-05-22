//! Useful utility functions for loading and initialising OpenGL shader programs
#pragma once

#include "GLHeaders.hpp"
#include <iostream>
#include <fstream>
#include <string>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/// <summary>
/// Compiles a GLSL shader.
/// </summary>
///
/// <param name="src">The shader's source string.</param>
/// <param name="shaderType">The type of shader (e.g. GL_VERTEX_SHADER).</param>
GLuint compileShader(const char* src, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar**)&src, NULL);
    glCompileShader(shader);

    // Check that the shader successfully compiled
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        // Get the size of the log message
        GLint logSize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

        // Get the log message
        char* logMsg = new char[logSize];
        glGetShaderInfoLog(shader, logSize, NULL, logMsg);

        std::cerr << "Failed to compile shader: " << logMsg << std::endl;
        delete[] logMsg;

        exit(EXIT_FAILURE);
    }

    return shader;
}

/// <summary>
/// Loads and compiles a GLSL shader from a file.
/// </summary>
///
/// <param name="filename">The shader's filename.</param>
/// <param name="shaderType">The type of shader (e.g. GL_VERTEX_SHADER).</param>
GLuint shaderFromFile(const std::string& filename, GLenum shaderType) {
    std::ifstream file(filename.c_str());

    // Check that the file was opened
    if (!file.is_open()) {
        std::cerr << "Failed to read: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    // Find the size of the file
    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());

    // Read the file into a buffer
    std::string buffer(size, ' ');
    file.seekg(0);
    file.read(&buffer[0], size);

    // Close the file
    file.close();

    return compileShader(buffer.c_str(), shaderType);
}

/// <summary>
/// Return a GLSL program object using compiled vertex and fragment shaders.
/// </summary>
///
/// <param name="vertexShader">The vertex shader to use in the program.</param>
/// <param name="fragmentShader">The fragment shader to use in the program.</param>
GLuint initProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check if the program successfully linked. If the program failed to link, get the error
    // message to show to the user.
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != (GLint)GL_TRUE) {
        // Get log length
        GLint logSize;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

        // Get the log message
        char* logMsg = new char[logSize];
        glGetProgramInfoLog(program, logSize, NULL, logMsg);

        std::cerr << "Failed to link shader program: " << logMsg << std::endl;
        delete[] logMsg;

        exit(EXIT_FAILURE);
    }
    return program;
}

