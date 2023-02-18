#version 330 core

in vec2 frag_UV_coords;

out vec4 frag_color;

uniform sampler2D texture_atlas;

void main() {
    frag_color = texture(texture_atlas, frag_UV_coords);
    if (frag_color.a < 0.1)
        discard;
}
