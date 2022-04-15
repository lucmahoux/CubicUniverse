#ifndef CUB_BLOCK_BLOCK_LIST_LOADER
#define CUB_BLOCK_BLOCK_LIST_LOADER

#include "block/block_utils.h"
#include "block/blockstates.h"

/* Load the block_list & initialize it. It sets the pointer 'nb_buffers' to
 * the number of 'RBO_chunks' loaded. It sets the pointer to 'RBO_chunks' as
 * well as the pointer to 'custom_block_RBO' accordingly */
void BLloader_load(BlockList* block_list,
                   struct cubCustomBlockInfoRBO** RBO_chunks,
                   RenderBufferObject* custom_block_RBO);

void BLloader_free_block_list(BlockList* block_list);

#ifdef CUB_DEV_STAGE
// [!] ALL the buffers MUST be added FIRST -
//     only then it is possible to add blocks [!]

/* Create or reset the block_list file if it exists */
void BLloader_create_block_list_file();

/* Adds a new rendering buffer in the block_list file */
void BLloader_add_buffer(GLfloat* VBO, uint16_t VBO_len,
                         GLuint* EBO, uint8_t EBO_len);

/* Adds a new block to the block_list file */
void BLloader_add_block(const char* name, RenderType render_type,
                        uint16_t info_RBO_id, BlockFlags block_flags,
                        uint8_t nb_states, bs_key* bs_keys,
                        bs_val* bs_default_values);
#endif

#endif
