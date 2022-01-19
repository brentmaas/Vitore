#version 460 core

layout(location = 0) in vec4 vertexPosition;
//layout(location = 1) in vec4 vertexColour;

uniform mat4 MVP;

//out vec4 fragmentColour;

void main(){
    gl_Position = MVP * vertexPosition;
    //fragmentColour = vertexColour;
}