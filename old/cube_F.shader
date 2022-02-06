#version 330 core

out vec4 frag_color;
in vec2 t_coords;

uniform sampler2D grass;
uniform sampler2D smiley;

void main() {
    frag_color = mix(texture(grass, t_coords), texture(smiley, t_coords), 0.8f);
}
