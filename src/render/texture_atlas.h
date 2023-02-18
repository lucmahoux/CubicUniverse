#ifndef CUB_RENDER_TEXTURE_ATLAS_H
#define CUB_RENDER_TEXTURE_ATLAS_H

#include "types/texture.h"
#include "open_gl.h"
#include "stb_image.h"

/* Must be called when the texture_atlas needs resetting */
void setup_texture_atlas(TextureAtlas* atlas, size_t fixed_width);

/* Adds the texture with 'texture_name' to the texture atlas.
 * The memory pointed to by 'texture' is set to the
 * bottom-left corner of the quad texture */
bool texture_atlas_add(TextureAtlas* atlas, const char* texture_name,
                       TexAtlasQuad* texture);

/* Called when all the images have been added to the atlas.
 * [!] The 'pixels' array is freed since it is copied to the GPU [!] */
void texture_atlas_bind_load_GPU(TextureAtlas* texture_atlas);

/* Set the shader uniforms related to the texture atlas via the uniform
 * locations provided - the corresponding shader must be used when calling */
void texture_atlas_set_uniforms(TextureAtlas* texture_atlas,
                                GLint texture_atlas_sampler_loc,
                                GLint texture_atlas_dim_vec2_loc,
                                GLint texel_padding_vec2_loc);
#endif
