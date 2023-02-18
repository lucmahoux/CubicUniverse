#version 330 core

layout (location = 0) in uvec2 packed_data;

out VS_OUT {
    uvec2 packed_data;
} vs_out;

// Position of the subchunk which is being renderered in chunk coordinates
// e.g: If it equals (1, 2, 3) is means that it refers to the subchunk which
// bottom-back-left corner is located at world coordinates (16, 32, 48)
uniform vec3 subchunk_pos;

/* --------------------------------------------------------------------------
 * SUCHUNK_X field : from 0 to 15
 * => X-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_X_MASK = 0x0000001Eu;
const uint SUBCHUNK_X_SHIFT = 1u;

/* --------------------------------------------------------------------------
 * SUCHUNK_Y field : from 0 to 15
 * => Y-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_Y_MASK = 0xF0000000u;
const uint SUBCHUNK_Y_SHIFT = 28u;

/* --------------------------------------------------------------------------
 * SUCHUNK_Z field : from 0 to 15
 * => Z-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_Z_MASK = 0x0F000000u;
const uint SUBCHUNK_Z_SHIFT = 24u;

void main() {
    vec3 block_sc_pos =
        vec3(float((packed_data[0] & SUBCHUNK_X_MASK) >> SUBCHUNK_X_SHIFT),
             float((packed_data[1] & SUBCHUNK_Y_MASK) >> SUBCHUNK_Y_SHIFT),
             float((packed_data[1] & SUBCHUNK_Z_MASK) >> SUBCHUNK_Z_SHIFT));
    gl_Position = vec4(subchunk_pos * 16.0 + block_sc_pos, 1.0);
    vs_out.packed_data = packed_data;
}
