#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fmt/ostream.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <string_view>

#include "shader.hpp"
#include "shader.frag.h"
#include "shader.vert.h"

void run(GLFWwindow* window, int width, int height) {
    auto program = ShaderProgram({
        {shaders_shader_vert, sizeof(shaders_shader_vert), GL_VERTEX_SHADER},
        {shaders_shader_frag, sizeof(shaders_shader_frag), GL_FRAGMENT_SHADER}
    });
    program.use();

    glm::mat4 projection = glm::perspective(45.0f, ((float) width) / height, 0.01f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    //Matrix components in vertex shader
    /*for(size_t i = 0;i < 4;++i) for(size_t j = 0;j < 4;++j){
        fmt::print(std::cout, "{}\n", mvp[i][j]);
    }*/
    GLuint mvpID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

    glEnable(GL_MULTISAMPLE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    float vertex[] = {-2, -2, 0, 1, 2, -2, 0, 1, 0, 2, 0, 1};
    float colour[] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);

    GLuint colourBuffer;
    glGenBuffers(1, &colourBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 12 * sizeof(float), colour, GL_STATIC_DRAW);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertexBuffer);
}

int main(int argc, char** argv) {
    bool borderless = argc > 1 && std::string(argv[1]) == "borderless";

    int width = 1200, height = 800;

    if (!glfwInit()) {
        fmt::print(std::cerr, "Could not initialize GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window;
    if (borderless) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(width,height, "SPIR-V test", monitor, NULL);
    } else {
        window = glfwCreateWindow(width, height, "SPIR-V test", NULL, NULL);
    }

    if (window == NULL) {
        fmt::print(std::cerr, "Could not create window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum type, GLenum, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
        fmt::print(std::cerr, "[OpenGL] {}\n", std::string_view(message, length));
    }, nullptr);

    run(window, width, height);

    glfwTerminate();

    return 0;
}
