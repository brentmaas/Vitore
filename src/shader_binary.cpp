#include "shader_binary.hpp"

#include <vector>
#include <iostream>

GLuint ShaderBinary::load() const {
    GLuint shaderId = glCreateShader(this->type);
    glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V, this->binary, this->length);
    glSpecializeShader(shaderId, "main", 0, nullptr, nullptr);

    GLint result = GL_FALSE;
    int infoLogLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!result){
        std::vector<char> errMsg(infoLogLength + 1);
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, errMsg.data());
        std::cerr << "Could not compile shader:" << std::endl;
        std::cerr << errMsg.data() << std::endl;
        return 0;
    }

    return shaderId;
}

GLuint loadShaderProgram(std::span<const ShaderBinary> shaders) {
    std::vector<GLuint> ids(shaders.size());
    for(size_t i = 0;i < shaders.size();++i){
        ids[i] = shaders[i].load();
        if (ids[i] == 0)
            return 0;
    }

    GLuint programId = glCreateProgram();

    for(size_t i = 0;i < shaders.size();++i){
        glAttachShader(programId, ids[i]);
    }

    glLinkProgram(programId);

    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!result){
        std::vector<char> errMsg(infoLogLength + 1);
        glGetProgramInfoLog(programId, infoLogLength, NULL, errMsg.data());
        std::cerr << "Could not link program:" << std::endl;
        std::cerr << errMsg.data() << std::endl;
        return 0;
    }

    for(size_t i = 0;i < shaders.size();++i){
        glDetachShader(programId, ids[i]);
        glDeleteShader(ids[i]);
    }

    return programId;
}
