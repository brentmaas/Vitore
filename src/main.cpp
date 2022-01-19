#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint loadShader(const char* file, GLuint type){
    GLuint shaderID = glCreateShader(type);
    
    std::string shaderSrc;
    std::ifstream shaderStream(file, std::ios::in);
    if(shaderStream.is_open()){
        std::stringstream strStream;
        strStream << shaderStream.rdbuf();
        shaderSrc = strStream.str();
        shaderStream.close();
    }else{
        std::cerr << "Could not open " << file << std::endl;
        return 0;
    }
    
    const char* srcPtr = shaderSrc.c_str();
    glShaderSource(shaderID, 1, &srcPtr, NULL);
    glCompileShader(shaderID);
    
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!result){
        char errMsg[infoLogLength+1] = "";
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, errMsg);
        std::cerr << errMsg << std::endl;
    }
    
    return shaderID;
}

GLuint loadProgram(const char* vertexFile, const char* fragmentFile){
    GLuint vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
    
    GLuint programID = glCreateProgram();
    
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    
    glLinkProgram(programID);
    
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!result){
        char errMsg[infoLogLength+1] = "";
        glGetProgramInfoLog(programID, infoLogLength, NULL, errMsg);
        std::cerr << errMsg << std::endl;
    }
    
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return programID;
}

int main(int argc, char** argv){
    bool borderless = argc > 1 && std::string(argv[1]) == "borderless";
    
    int width = 1200, height = 800;
    
    if(!glfwInit()){
        std::cerr << "Could not initialise GLFW" << std::endl;
        return 1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    GLFWwindow* window;
    if(borderless){
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(width,height, "OpenGL 4.6 Triangle", monitor, NULL);
    }else{
        window = glfwCreateWindow(width, height, "OpenGL 4.6 Triangle", NULL, NULL);
    }
    
    if(window == NULL){
        std::cerr << "Could not create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    
    glfwSwapInterval(1);
    
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    GLuint program = loadProgram("shaders/shader.vert", "shaders/shader.frag");
    glUseProgram(program);
    
    glm::mat4 projection = glm::perspective(45.0f, ((float) width) / height, 0.01f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    GLuint mvpID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
    
    glEnable(GL_MULTISAMPLE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    float vertex[] = {-2, -2, 0, 1, 2, -2, 0, 1, 0, 2, 0, 1};
    //float colour[] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1};
    
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);
    
    /*GLuint colourBuffer;
    glGenBuffers(1, &colourBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 12 * sizeof(float), colour, GL_STATIC_DRAW);*/
    
    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        //glEnableVertexAttribArray(1);
        //glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glDrawArrays(GL_POINTS, 0, 3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteBuffers(1, &vertexBuffer);
    //glDeleteBuffers(1, &colourBuffer);
    glDeleteProgram(program);
    
    glfwTerminate();
    
    return 0;
}