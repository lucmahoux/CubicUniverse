#version 330 core

out vec4 frag_color;
in vec2 tex_coords;
in float face_ID;

uniform sampler2D tex_top;
uniform sampler2D tex_bottom;
uniform sampler2D tex_front;
uniform sampler2D tex_back;
uniform sampler2D tex_left;
uniform sampler2D tex_right;

void main() {
    if (face_ID == 1.0f)
        frag_color = texture(tex_top, tex_coords);
    else if (face_ID == 2.0f)
        frag_color = texture(tex_bottom, tex_coords);
    else if (face_ID == 3.0f)
        frag_color = texture(tex_front, tex_coords);
    else if (face_ID == 4.0f)
        frag_color = texture(tex_back, tex_coords);
    else if (face_ID == 5.0f)
        frag_color = texture(tex_left, tex_coords);
    else
        frag_color = texture(tex_right, tex_coords);
    if (frag_color.a < 0.1)
        discard;
}
