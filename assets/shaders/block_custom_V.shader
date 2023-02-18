#version 330 core

layout (location = 0) in vec3 local_position;
layout (location = 1) in vec2 local_UV_coords_vertex;
layout (location = 2) in float vertex_texture_ID; // 0 to 5

out vec2 frag_UV_coords;

// Indices 0 to 2 => TEXTURES_Y_OFFSET (16 bits per textures)
// Indice 3 => TEXTURES_X_OFFSET (5 bits per texture)
// Indice 4 => TEXTURES_TYPE (3 bits per texture)
uniform uint texture_data[5];
uniform uint packed_data;

// Position of the subchunk which is being renderered in chunk coordinates
// e.g: If it equals (1, 2, 3) is means that it refers to the subchunk which
// bottom-back-left corner is located at world coordinates (16, 32, 48)
uniform vec3 subchunk_pos;

uniform vec2 texture_atlas_dim; // dimensions of the atlas but divided by 16

uniform mat4 VP_matrix;

/* primary_rotations[0] = MIDDLE_Y_90
 * primary_rotations[1] = MIDDLE_Y_180
 * primary_rotations[2] = MIDDLE_Y_270 */
uniform mat4 primary_rotations[3];

/* secondary_rotations[0] = LEFT_Z_DOWN_90
 * secondary_rotations[1] = LEFT_Z_DOWN_270
 * secondary_rotations[2] = RIGHT_Z_DOWN_90
 * secondary_rotations[3] = RIGHT_Z_DOWN_270
 * secondary_rotations[4] = BACK_X_DOWN_90
 * secondary_rotations[5] = BACK_X_DOWN_270
 * secondary_rotations[6] = FRONT_X_DOWN_90
 * secondary_rotations[7] = FRONT_X_DOWN_270 */
uniform mat4 secondary_rotations[8];

/* --------------------------------------------------------------------------
 * TEX_OFFSET_X field : from 0 to 31
 * => horizontal offset of 16 pixels-groups from the TOP-LEFT corner
 * of the texture to render (e.g. if TL pixel X at index 31, then offset is 2)
 * -------------------------------------------------------------------------- */
const uint TEX_OFFSET_X_MASK = 0x0000001Fu;
const uint TEX_OFFSET_X_BASE_SHIFT = 5u;

/* --------------------------------------------------------------------------
 * TEX_OFFSET_Y field : from 0 to 65535
 * => vertical offset of 16 pixels-groups from the TOP-LEFT corner
 * of the texture to render (e.g. if TL pixel Y at index 31, then offset is 2)
 * -------------------------------------------------------------------------- */
const uint TEX_OFFSET_Y_MASK = 0x0000FFFFu;
const uint TEX_OFFSET_Y_BASE_SHIFT = 16u;

/* --------------------------------------------------------------------------
 * TEX_TYPE field : from 0 to 5
 * 16x16 (0) - 32x32 (1) - 64x64 (2) - 128x128 (3) - 256x256 (4) - 512x512 (5)
 * -------------------------------------------------------------------------- */
const uint TEX_TYPE_MASK = 0x00000007u;
const uint TEX_TYPE_BASE_SHIFT = 3u;

/* --------------------------------------------------------------------------
 * SUCHUNK_X field : from 0 to 15
 * => X-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_X_MASK = 0xF0000000u;
const uint SUBCHUNK_X_SHIFT = 28u;

/* --------------------------------------------------------------------------
 * SUCHUNK_Y field : from 0 to 15
 * => Y-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_Y_MASK = 0x0F000000u;
const uint SUBCHUNK_Y_SHIFT = 24u;

/* --------------------------------------------------------------------------
 * SUCHUNK_Z field : from 0 to 15
 * => Z-axis offset position of the block to render in the subchunk
 * relative to its bottom-back-left corner
 * -------------------------------------------------------------------------- */
const uint SUBCHUNK_Z_MASK = 0x00F00000u;
const uint SUBCHUNK_Z_SHIFT = 20u;

/* --------------------------------------------------------------------------
 * CUBE_HALF field : 0 or 1
 * If equal to 1, all local coords U are recalculated as (0, 1, 0) - U
 * It is used when a block does not cover the cube space entirely
 * (e.g. trapdoors).
 * -------------------------------------------------------------------------- */
const uint CUBE_HALF_MASK = 0x00080000u;
const uint CUBE_HALF_SHIFT = 19u;

/* --------------------------------------------------------------------------
 * PRIMARY_ROTATION field : from 0 to 3
 * NO_MIDDLE_ROT (0) - MIDDLE_Y_90 (1) - MIDDLE_Y_180 (2) - MIDDLE_Y_270 (3)
 * -------------------------------------------------------------------------- */
const uint PRIMARY_ROTATION_MASK = 0x00060000u;
const uint PRIMARY_ROTATION_SHIFT = 17u;

/* --------------------------------------------------------------------------
 * SECONDARY_ROTATION field : from 0 to 8
 * NO_SECONDARY_ROTATION (0) - LEFT_Z_DOWN_90 (1) - LEFT_Z_DOWN_270 (2) -
 * RIGHT_Z_DOWN_90 (3) - RIGHT_Z_DOWN_270 (4) - BACK_X_DOWN_90 (5) -
 * BACK_X_DOWN_270 (6) - FRONT_X_DOWN_90 (7) - FRONT_X_DOWN_270 (8)
 * -------------------------------------------------------------------------- */
const uint SECONDARY_ROTATION_MASK = 0x0001E000u;
const uint SECONDARY_ROTATION_SHIFT = 13u;

void set_UV_coords_from_face_ID(out vec2 frag_UV_coords) {
    uint tex_ID = uint(vertex_texture_ID);
    uint tex_type = (texture_data[4] >> (TEX_TYPE_BASE_SHIFT * tex_ID))
                    & TEX_TYPE_MASK;
    float tex_side = 16.0 * float(1u << tex_type) - 1.0;
    vec2 BL_corner;
    BL_corner.x = float(
            (texture_data[3] >> (TEX_OFFSET_X_BASE_SHIFT * tex_ID))
            & TEX_OFFSET_X_MASK
            ) * 16.0;
    BL_corner.y = float(
            (texture_data[tex_ID / 2u] >> (TEX_OFFSET_Y_BASE_SHIFT * (tex_ID % 2u)))
            & TEX_OFFSET_Y_MASK
            ) * 16.0 + tex_side;
    vec2 wanted_texel = vec2(tex_side, -tex_side) * local_UV_coords_vertex;
    wanted_texel = vec2(floor(wanted_texel.x), floor(wanted_texel.y));
    frag_UV_coords = (BL_corner + wanted_texel + 0.5)
                     / (texture_atlas_dim * 16.0);
}

void main() {
    vec3 block_sc_pos =
        vec3(float((packed_data & SUBCHUNK_X_MASK) >> SUBCHUNK_X_SHIFT),
             float((packed_data & SUBCHUNK_Y_MASK) >> SUBCHUNK_Y_SHIFT),
             float((packed_data & SUBCHUNK_Z_MASK) >> SUBCHUNK_Z_SHIFT));
    vec4 local_coords = vec4(local_position, 1.0);

    uint primary_rotation_ID = (packed_data & PRIMARY_ROTATION_MASK)
                               >> PRIMARY_ROTATION_SHIFT;
    if (primary_rotation_ID != 0u)
        local_coords *= primary_rotations[primary_rotation_ID - 1u];

    uint secondary_rotation_ID = (packed_data & SECONDARY_ROTATION_MASK)
                                 >> SECONDARY_ROTATION_SHIFT;
    if (secondary_rotation_ID != 0u)
        local_coords *= secondary_rotations[secondary_rotation_ID - 1u];

    uint cube_half = (packed_data & CUBE_HALF_MASK) >> CUBE_HALF_SHIFT;
    if (cube_half != 0u)
        local_coords.y = 1.0 - local_coords.y;

    gl_Position = VP_matrix * vec4(local_coords.xyz + subchunk_pos * 16.0
                                    + block_sc_pos, 1.0);
    set_UV_coords_from_face_ID(frag_UV_coords);
}
