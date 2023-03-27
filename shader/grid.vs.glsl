#version 460

in vec3 vertexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;

void main()
{
    gl_Position =  mvp * vec4(vertexPos, 1.0);
}