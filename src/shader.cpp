#include "shader.hpp"

#include <utility>
#include <vector>
#include <cassert>

namespace {
    using Shader = UniqueHandle<[](GLuint shader) { glDeleteShader(shader); }>;

    Shader compileShader(const ShaderProgram::ShaderSource& shader_source) {
        auto shader = Shader(glCreateShader(shader_source.type));

        glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shader_source.binary, shader_source.length);
        glSpecializeShader(shader, "main", 0, nullptr, nullptr);

        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            int info_log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
            auto msg = std::string(info_log_length, 0);
            glGetShaderInfoLog(shader, info_log_length, NULL, msg.data());
            throw ShaderProgram::CompileError{std::move(msg)};
        }

        return shader;
    }
}

ShaderProgram::ShaderProgram(std::initializer_list<ShaderSource> sources):
    ShaderProgram(std::span<const ShaderSource>(sources.begin(), sources.end())) {}

ShaderProgram::ShaderProgram(std::span<const ShaderSource> sources):
    program(glCreateProgram()) {

    auto shaders = std::vector<Shader>();
    shaders.reserve(sources.size());
    for (const auto& source : sources) {
        shaders.push_back(compileShader(source));
        glAttachShader(this->program, shaders.back());
    }

    glLinkProgram(this->program);

    GLint ok = GL_FALSE;
    glGetProgramiv(this->program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint info_log_length;
        glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &info_log_length);
        auto msg = std::string(info_log_length, 0);
        glGetProgramInfoLog(this->program, info_log_length, NULL, msg.data());
        throw ShaderProgram::LinkError{std::move(msg)};
    }

    for (const auto& shader : shaders) {
        glDetachShader(this->program, shader);
    }
}

void ShaderProgram::use() const {
    assert(this->program != 0);
    glUseProgram(this->program);
}

ShaderProgram::operator GLuint() const {
    return this->program;
}
