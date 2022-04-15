#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

flat out vec3 frag_color;

uniform mat4 VP_matrix;

void main()
{
    frag_color = color;
    gl_Position = VP_matrix * vec4(vertex, 1.0);
}
