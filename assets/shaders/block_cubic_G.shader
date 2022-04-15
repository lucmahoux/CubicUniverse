#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    uvec2 packed_data;
} gs_in[];

out vec2 frag_UV_coords;

uniform mat4 VP_matrix;
uniform vec2 texture_atlas_dim; // dimensions of the atlas but divided by 16
uniform vec2 texel_padding; // See "Half pixel correction" for more details

/* --------------------------------------------------------------------------
 * FACE_ID field : from 0 to 5
 * TOP (0) - BOTTOM (1) - FRONT (2) - BACK (3) - LEFT (4) - RIGHT (5)
 * -------------------------------------------------------------------------- */
const uint FACE_ID_MASK = 0xE0000000u;
const int FACE_ID_SHIFT = 29;

/* --------------------------------------------------------------------------
 * TEXTURE_OFFSET_X field : from 0 to 31
 * => horizontal offset of 16 pixels-groups from the TOP-LEFT corner
 * of the texture to render (e.g: if TL pixel X at index 31, then offset is 2)
 * -------------------------------------------------------------------------- */
const uint TEX_OFFSET_X_MASK = 0x1F000000u;
const int TEX_OFFSET_X_SHIFT = 24;

/* --------------------------------------------------------------------------
 * TEXTURE_OFFSET_Y field : from 0 to 65535
 * => vertical offset of 16 pixels-groups from the TOP-LEFT corner
 * of the texture to render (e.g: if TL pixel Y at index 31, then offset is 2)
 * -------------------------------------------------------------------------- */
const uint TEX_OFFSET_Y_MASK = 0x00FFFF00u;
const int TEX_OFFSET_Y_SHIFT = 8;

/* --------------------------------------------------------------------------
 * TEXTURE_TYPE field : from 0 to 5
 * 16x16 (0) - 32x32 (1) - 64x64 (2) - 128x128 (3) - 256x256 (4) - 512x512 (5)
 * -------------------------------------------------------------------------- */
const uint TEX_TYPE_MASK = 0x000000E0u;
const int TEX_TYPE_SHIFT = 5;

void set_face_vertices(out vec4 vertices[4]) {
    uint face_ID = (gs_in[0].packed_data[0] & FACE_ID_MASK) >> FACE_ID_SHIFT;
    if (face_ID == 0u) {
        vertices[0] = gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0);
        vertices[1] = gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0);
        vertices[2] = gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0);
    } else if (face_ID == 1u) {
        vertices[0] = gl_in[0].gl_Position;
        vertices[1] = gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0);
        vertices[2] = gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0);
    } else if (face_ID == 2u) {
        vertices[0] = gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0);
        vertices[1] = gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0);
        vertices[2] = gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0);
    } else if (face_ID == 3u) {
        vertices[0] = gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0);
        vertices[1] = gl_in[0].gl_Position;
        vertices[2] = gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0);
    } else if (face_ID == 4u) {
        vertices[0] = gl_in[0].gl_Position;
        vertices[1] = gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0);
        vertices[2] = gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0);
    } else if (face_ID == 5u) {
        vertices[0] = gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0);
        vertices[1] = gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0);
        vertices[2] = gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0);
        vertices[3] = gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0);
    }
}

void set_face_UV_coords(out vec2 UV_coords[4]) {
    uint tex_type = (gs_in[0].packed_data[0] & TEX_TYPE_MASK) >> TEX_TYPE_SHIFT;
    vec2 TL_corner =
        vec2(float((gs_in[0].packed_data[0] & TEX_OFFSET_X_MASK)
                    >> TEX_OFFSET_X_SHIFT) / texture_atlas_dim.x,
             float((gs_in[0].packed_data[0] & TEX_OFFSET_Y_MASK)
                >> TEX_OFFSET_Y_SHIFT) / texture_atlas_dim.y);
    vec2 side = vec2(float(1 << tex_type), float(1 << tex_type))
                / texture_atlas_dim;
    UV_coords[3] = TL_corner + texel_padding;
    UV_coords[2] = TL_corner + vec2(side.x - texel_padding.x, texel_padding.y);
    UV_coords[1] = TL_corner + side - texel_padding;
    UV_coords[0] = TL_corner + vec2(texel_padding.x, side.y - texel_padding.y);
}

void main() {
    vec4 vertices[4];
    set_face_vertices(vertices);
    vec2 UV_coords[4];
    set_face_UV_coords(UV_coords);

    gl_Position = VP_matrix * vertices[0];
    frag_UV_coords = UV_coords[0];
    EmitVertex();

    gl_Position = VP_matrix * vertices[1];
    frag_UV_coords = UV_coords[1];
    EmitVertex();

    gl_Position = VP_matrix * vertices[3];
    frag_UV_coords = UV_coords[3];
    EmitVertex();

    gl_Position = VP_matrix * vertices[2];
    frag_UV_coords = UV_coords[2];
    EmitVertex();

    EndPrimitive();
}
