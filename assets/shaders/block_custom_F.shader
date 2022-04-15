#version 330 core

out vec4 frag_color;
in vec2 frag_UV_coords;

uniform sampler2D texture_atlas;

void main() {
    frag_color = texture(texture_atlas, frag_UV_coords);
    if (frag_color.a < 0.1)
        discard;
}
