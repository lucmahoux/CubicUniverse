#include "block_list_loader.h"
#include "fs.h"
// ----------------------------------------------------------------------------
/* Determine the number of .png textures to load for one draw call
 * This number is equivalent to the field cubBlockData.nb_tex_draw */
static uint8_t BLloader_count_draw_tex(RenderType type);

/* Allocates the correct amount of memory to block->textures based
 * on the number of textures to load per draw call and the number
 * of "modes" its blockstates(BS) implies.
 * e.g: for a flower, nb_tex_draw = 1 (render_type == RT_DEFAULT)
 * but the BS BS_FLOWER_TYPE has 11 modes.
 * Therefore, the memory allocated must be nb_tex_draw * 11 * sizeof(GLuint) */
static void BLloader_allocate_tex_mem(BlockData* block);

/* Initialize the cubRenderBufferObject render_obj with the VBO & EBO buffers */
static void BLloader_create_custom_block_RBO(GLfloat* VBO, GLuint* EBO,
                                      GLsizei VBO_len, GLsizei EBO_len,
                                      RenderBufferObject* render_obj);

/* Load the header of the block list file & set the 'nb_buffers' & 'RBO_chunks'
 * values accordingly.
 * The memory for the array block_list->blocks is also allocated */
static void BLloader_load_header(BlockList* block_list,
                          struct cubCustomBlockInfoRBO** RBO_chunks,
                          RenderBufferObject* RBO, FILE* fp);

/* Load the body (= the list of blocks) of the block list file */
static void BLloader_load_blocks(BlockList* block_list, FILE* fp);

// ----------------------------------------------------------------------------

static uint8_t BLloader_count_draw_tex(RenderType type) {
    uint8_t count = 0;
    if (type & RT_DEFAULT) ++count;
    if (type & RT_TOP) ++count;
    if (type & RT_BOTTOM) ++count;
    if (type & RT_SIDE) ++count;
    if (type & RT_FRONT) ++count;
    if (type & RT_BACK) ++count;
    if (type & RT_LEFT) ++count;
    if (type & RT_RIGHT) ++count;
    return count;
}

static void BLloader_allocate_tex_mem(BlockData* block) {
    uint16_t nb_modes = block->flags.has_bs_tex
                ? BS_TEX_get_nb_modes(CUB_BSK_TEX(block))
                : 1;
    block->textures = utils_malloc(nb_modes * block->nb_tex_draw *
                                   sizeof(TexAtlasQuad),
                                   "BLloader_allocate_tex_mem");
    printf("BLloader_allocate_tex_mem: Allocated %u texAtlasQuad for"
            " block ID: %i!\n", nb_modes * block->nb_tex_draw, block->id);
}

static void BLloader_create_custom_block_RBO(GLfloat* VBO, GLuint* EBO,
                                      GLsizei VBO_len, GLsizei EBO_len,
                                      RenderBufferObject* render_obj) {
    // Generate the buffers
    setup_render_buffer_object(render_obj);
    glGenBuffers(1, &render_obj->VBO);
    glGenBuffers(1, &render_obj->EBO);
    glGenVertexArrays(1, &render_obj->VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(render_obj->VAO);
    // 1) Vertex Buffers
    glBindBuffer(GL_ARRAY_BUFFER, render_obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, VBO_len * sizeof(GLfloat),
                 VBO, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // Coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*) 0);

    glEnableVertexAttribArray(1); // Local UV Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2); // Face identifier (NSEWTB)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    // 2) Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBO_len * sizeof(GLuint),
                 EBO, GL_STATIC_DRAW);
    render_obj->nb_elements = EBO_len;

    // Unbind everything [!] Order of unbinding matters [!]
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void BLloader_load_header(BlockList* block_list,
                          struct cubCustomBlockInfoRBO** RBO_chunks,
                          RenderBufferObject* RBO, FILE* fp) {
    const char fname[] = "BLloader_load";
    // Get the number of rendering buffers & the number of blocks
    uint8_t nb_buffers;
    UTILS_FREAD(&nb_buffers, sizeof(uint8_t), 1, fp, fname, "nb_buffers");
    UTILS_FREAD(&block_list->nb_blocks, sizeof(BDuid), 1,
                fp, fname, "nb_blocks");
    // Allocate the Render Buffer Object (RBO) & cubBlockData array accordingly
    GLfloat* VBO = NULL; GLfloat* target_VBO = NULL;
    GLuint* EBO = NULL; GLuint* target_EBO = NULL;
    GLsizei total_VBO_len = 0, total_EBO_len = 0;
    block_list->blocks = utils_malloc(block_list->nb_blocks * sizeof(BlockData),
                                      fname);
    *RBO_chunks = utils_malloc(nb_buffers *
                               sizeof(struct cubCustomBlockInfoRBO), fname);
    for (uint16_t i = 0; i < nb_buffers; ++i) {
        struct cubCustomBlockInfoRBO* RBO_chunk = *RBO_chunks + i;
        GLsizei VBO_len;
        UTILS_FREAD(&VBO_len, sizeof(GLsizei), 1, fp, fname, "VBO_len");
        VBO = utils_realloc(VBO, (total_VBO_len + VBO_len) * sizeof(GLfloat),
                            fname);
        target_VBO = VBO + total_VBO_len;
        UTILS_FREAD(target_VBO, sizeof(GLfloat),
                    (size_t) VBO_len, fp, fname, "VBO");
        UTILS_FREAD(&RBO_chunk->nb_elements, sizeof(GLsizei),
                    1, fp, fname, "EBO_len");
        RBO_chunk->element_offset = total_EBO_len;
        EBO = utils_realloc(EBO, (total_EBO_len + RBO_chunk->nb_elements)
                                 * sizeof(GLuint), fname);
        target_EBO = EBO + total_EBO_len;
        UTILS_FREAD(target_EBO, sizeof(GLuint),
                    (size_t) RBO_chunk->nb_elements, fp, fname, "EBO");
        for (GLsizei k = 0; k < RBO_chunk->nb_elements; ++k)
            EBO[RBO_chunk->element_offset + k] += total_VBO_len;
        total_EBO_len += RBO_chunk->nb_elements;
        total_VBO_len += VBO_len;
    }
    BLloader_create_custom_block_RBO(VBO, EBO, total_VBO_len,
                                     total_EBO_len, RBO);
    free(VBO);
    free(EBO);
}

static void BLloader_load_blocks(BlockList* block_list, FILE* fp) {
    const char fname[] = "BLloader_load_blocks";
    BSuid origin_BS_uid = 0;
    for (uint32_t i = 0; i < block_list->nb_blocks; ++i) {
        BlockData* current = &block_list->blocks[i];
        UTILS_FREAD(&current->name, sizeof(char), MAX_BLOCK_STRLEN,
                    fp, fname, "name"); // Name
        UTILS_FREAD(&current->id, sizeof(BDuid), 1,
                    fp, fname, "id"); // ID
        UTILS_FREAD(&current->render_type, sizeof(RenderType), 1,
                    fp, fname, "render_type"); // Render type
        current->nb_tex_draw = BLloader_count_draw_tex(current->render_type);
        UTILS_FREAD(&current->info_RBO_id, sizeof(uint16_t), 1,
                    fp, fname, "info_RBO_id"); // INFO RBO ID
        UTILS_FREAD(&current->flags, sizeof(BlockFlags), 1,
                    fp, fname, "block_flags"); // Block flags
        UTILS_FREAD(&current->nb_states, sizeof(uint8_t), 1,
                    fp, fname, "block_nb_states");
        current->origin_BS_uid = origin_BS_uid;
        BSuid bs_possible_combinations = 1;
        // Set up blockstates (if any)
        if (current->flags.has_states) {
            uint8_t nb_bs = current->nb_states;
            current->bs_keys = malloc(nb_bs * sizeof(bs_key));
            current->bs_default_values = malloc(nb_bs * sizeof(bs_val));
            if (!current->bs_keys || !current->bs_default_values)
                errx(1, "Malloc failed!");
            for (uint8_t k = 0; k < nb_bs; ++k) {
                UTILS_FREAD(&current->bs_keys[k], sizeof(bs_key), 1,
                            fp, fname, "blockstate_key");
                UTILS_FREAD(&current->bs_default_values[k],
                            sizeof(bs_val), 1, fp, fname,
                            "blockstate_default_value");
                bs_possible_combinations *= CUB_BSV_LEN[current->bs_keys[k]];
            }
            if (current->flags.has_bs_tex) {
                current->bs_name_parser = current->flags.is_bs_creator
                                ? BS_is_creator(CUB_BSK_TEX(current))
                                : BS_is_modificator(CUB_BSK_TEX(current));
            } else current->bs_name_parser = NULL;
        } else {
            current->bs_keys = NULL;
            current->bs_default_values = NULL;
            current->bs_name_parser = NULL;
        }
        origin_BS_uid += bs_possible_combinations;
        // Allocate memory for the textures
        BLloader_allocate_tex_mem(current);
    }
}

void BLloader_load(BlockList* block_list,
                   struct cubCustomBlockInfoRBO** RBO_chunks,
                   RenderBufferObject* custom_block_RBO) {
    printf("block_load_block_list: Trying to load %s...\n", BLOCK_LIST_FILE);
    FILE* fp;
    if ((fp = get_appdata_file(BLOCK_LIST_FILE, "rb")) == NULL ) {
        errx(1, "block_load_block_list: File not found: %s!", BLOCK_LIST_FILE);
    } else {
        BLloader_load_header(block_list, RBO_chunks, custom_block_RBO, fp);
        BLloader_load_blocks(block_list, fp);
        fclose(fp);
        printf("block_load_block_list: Block list loaded successfully!\n");
    }
}

void BLloader_free_block_list(BlockList* block_list) {
    printf("BLloader_free_block_list: Starting to free block list...\n");
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        BlockData* current = block_list->blocks + i;
        free(current->bs_keys);
        free(current->bs_default_values);
        free(current->textures);
    }
    free(block_list->blocks);
    printf("BLloader_free_block_list: Block list freed!\n");
}

void BLloader_create_block_list_file(void) {
    FILE* fp = get_appdata_file(BLOCK_LIST_FILE, "w");
    if (fp) fclose(fp);
}

void BLloader_add_buffer(GLfloat* VBO, GLsizei VBO_len,
                         GLuint* EBO, GLsizei EBO_len) {
    FILE* fp;
    if ((fp = get_appdata_file(BLOCK_LIST_FILE, "r+b")) != NULL ) {
        const char fname[] = "cub_BLloader_add_buffer";
        uint8_t nb_buffers = 0;
        if (fread(&nb_buffers, sizeof(uint8_t), 1, fp) != 1) {
            // New file
            rewind(fp);
            UTILS_FWRITE(&nb_buffers, sizeof(uint8_t), 1,
                         fp, fname, "nb_buffers");
            BDuid nb_blocks = 0;
            UTILS_FWRITE(&nb_blocks, sizeof(BDuid), 1, fp, fname, "nb_blocks");
        }
        fseek(fp, 0, SEEK_END); // Go to the end of the file
        UTILS_FWRITE(&VBO_len, sizeof(GLsizei), 1, fp, fname, "VBO_len");
        UTILS_FWRITE(VBO, sizeof(GLfloat), (size_t) VBO_len, fp, fname, "VBO");
        UTILS_FWRITE(&EBO_len, sizeof(GLsizei), 1, fp, fname, "EBO_len");
        UTILS_FWRITE(EBO, sizeof(GLuint), (size_t) EBO_len, fp, fname, "EBO");
        fseek(fp, 0, SEEK_SET); // Go to the start of the file
        ++nb_buffers; // Update the buffer counter
        UTILS_FWRITE(&nb_buffers, sizeof(uint8_t), 1, fp, fname, "nb_buffers");
        fclose(fp);
    } else errx(1, "cub_BLloader_add_buffer: failed to open %s!\n"
                   "Please create it so that it can be populated!", BLOCK_LIST_FILE);
}

void BLloader_add_block(const char* name, RenderType render_type,
                        uint16_t info_RBO_id, BlockFlags block_flags,
                        uint8_t nb_states, bs_key* bs_keys,
                        bs_val* bs_default_values) {
    FILE* fp;
    if ((fp = get_appdata_file(BLOCK_LIST_FILE, "r+b")) != NULL ) {
        const char fname[] = "cub_BLloader_add_block";
        fseek(fp, sizeof(uint8_t), SEEK_SET);
        BDuid nb_blocks;
        UTILS_FREAD(&nb_blocks, sizeof(BDuid), 1, fp, fname, "nb_blocks");
        fseek(fp, 0, SEEK_END);
        char buffer[MAX_BLOCK_STRLEN];
        uint8_t i = 0;
        for (; name[i] && i < MAX_BLOCK_STRLEN; ++i)
            buffer[i] = name[i];
        for (; i < MAX_BLOCK_STRLEN; ++i)
            buffer[i] = '\0';
        UTILS_FWRITE(buffer, sizeof(char), MAX_BLOCK_STRLEN,
                     fp, fname, "block_name");
        // Block ID is the previous number of blocks in the list
        UTILS_FWRITE(&nb_blocks, sizeof(BDuid), 1, fp, fname, "block_id");
        UTILS_FWRITE(&render_type, sizeof(RenderType), 1,
                     fp, fname, "block_render_type");
        UTILS_FWRITE(&info_RBO_id, sizeof(uint16_t), 1,
                     fp, fname, "info_RBO_id");
        UTILS_FWRITE(&block_flags, sizeof(BlockFlags), 1,
                     fp, fname, "block_flags");
        UTILS_FWRITE(&nb_states, sizeof(uint8_t), 1,
                     fp, fname, "block_nb_states");
        for (uint8_t j = 0; j < nb_states; ++j) {
            UTILS_FWRITE(bs_keys + j, sizeof(bs_key), 1,
                         fp, fname, "blockstate_key");
            UTILS_FWRITE(bs_default_values + j, sizeof(bs_val), 1,
                         fp, fname, "blockstate_value");
        }
        fseek(fp, sizeof(uint8_t), SEEK_SET);
        ++nb_blocks; // Update the block counter
        UTILS_FWRITE(&nb_blocks, sizeof(BDuid), 1, fp, fname, "nb_blocks");
        fclose(fp);
    } else errx(1, "BLloader_add_block: failed to open %s\n"
                   "Please create it so that it can be populated!", BLOCK_LIST_FILE);
}
