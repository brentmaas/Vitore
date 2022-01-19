#ifndef _VITORE_SHADER_BINARY_HPP
#define _VITORE_SHADER_BINARY_HPP

#include <glad/glad.h>

#include <span>
#include <cstddef>

struct ShaderBinary {
    const GLuint* binary;
    const GLsizei length;
    const GLuint type;

    GLuint load() const;
};

GLuint loadShaderProgram(std::span<const ShaderBinary> shaders);

#endif
