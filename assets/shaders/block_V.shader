#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coords;
layout (location = 2) in float vertex_face_ID;

out vec2 tex_coords;
out float face_ID;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
    gl_Position =   projection_matrix * view_matrix *
                    model_matrix * vec4(position, 1.0);
    tex_coords = texture_coords;
    face_ID = vertex_face_ID;
}
