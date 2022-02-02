#version 460 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColour;

layout(location = 0) uniform mat4 MVP;

layout(location = 0) out vec4 fragmentColour;

void main(){
    //gl_Position = MVP * vertexPosition;
    //Uniforms don't work; grabbed matrix components from main.cpp
    gl_Position = vec4(1.19506 * vertexPosition.x, 1.79259 * vertexPosition.y, -1.0002 * vertexPosition.z - vertexPosition.w, 4.981 * vertexPosition.z + 5 * vertexPosition.w);
    fragmentColour = vertexColour;
}
