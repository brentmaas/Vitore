#version 460 core

//in vec4 fragmentColour;

layout(location = 0) out vec4 colour;

void main(){
    colour = vec4(1, 1, 1, 1); //color = fragmentColour;
}
