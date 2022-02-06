#version 330 core

out vec4 frag_color;
in vec2 tex_coords;
in float face_ID;

uniform sampler2D tex_south;
uniform sampler2D tex_east;
uniform sampler2D tex_north;
uniform sampler2D tex_west;
uniform sampler2D tex_top;
uniform sampler2D tex_bottom;

void main() {
    if (face_ID == 1.0f)
        frag_color = texture(tex_south, tex_coords);
    else if (face_ID == 2.0f)
        frag_color = texture(tex_east, tex_coords);
    else if (face_ID == 3.0f)
        frag_color = texture(tex_north, tex_coords);
    else if (face_ID == 4.0f)
        frag_color = texture(tex_west, tex_coords);
    else if (face_ID == 5.0f)
        frag_color = texture(tex_top, tex_coords);
    else
        frag_color = texture(tex_bottom, tex_coords);
}
