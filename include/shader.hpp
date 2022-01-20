#ifndef _VITORE_SHADER_HPP
#define _VITORE_SHADER_HPP

#include "unique_handle.hpp"

#include <glad/glad.h>

#include <span>
#include <string>

struct ShaderProgram {
    struct CompileError {
        std::string msg;
    };

    struct LinkError {
        std::string msg;
    };

    struct ShaderSource {
        const GLuint* binary;
        const GLsizei length;
        const GLuint type;
    };

    UniqueHandle<[](GLuint program){ glDeleteProgram(program); }> program;

    explicit ShaderProgram(std::initializer_list<ShaderSource> shaders);
    explicit ShaderProgram(std::span<const ShaderSource> shaders);

    void use() const;

    operator GLuint() const;
};
#endif
