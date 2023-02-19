#ifndef CUB_CHUNK_TYPES_H
#define CUB_CHUNK_TYPES_H

#include "types/block.h"
#include "data_struct/graph_marker.h"
#include "data_struct/hashmap.h"
#include "data_struct/queue.h"
#include "utils.h"

#define CHUNK_WIDTH 16
#define CHUNK_DEPTH 16
#define CHUNK_HEIGHT 256
#define SUBCHUNK_HEIGHT 16
#define MAX_SUBCHUNKS 16
#define PALETTE_DEFAULT_LEN 30
#define PALETTE_DEFAULT_HASH_NBR 31
#define palette_id uint16_t

struct cubGridPosition {
    int16_t x; // Horizontal position of the chunk in the grid
    int16_t y; // Vertical position of the subchunk (0 to 15)
    int16_t z; // Vertical position of the chunk in the grid
};

struct cubRenderingDataBFS {
    struct cubGridPosition position; // Position of the subchunk in the grid
    FaceID src_face; // Face from which the BFS enqueued the subchunk
    vec3 directional_vector; // Vector between the source of the BFS & the SC
};

struct cubSubChunkPalette {
    // List of BP_elt objects consisting the palette
    List* BP_elts;
    // Hashmap linking each BlockState Unique ID (BS_uid) to the index
    // of its associated BP_elt in 'BP_elts' if it exists.
    HashMap* BS_uid_mapper;
};

typedef struct cubSubChunk {
    // Y position of the subchunk relative to the bottom of the chunk it belongs
    uint8_t y_pos;
    struct cubSubChunkPalette palette;
    // List of indices in 'palette' to determine the type of block
    palette_id* blocks;
    /* The visibility graph of a subchunk determines how it connects to its
     * neighboring subchunks. The graph is stored as an adjacency matrix
     * where the 2nd dimension is stored as an uint8_t. Therefore,
     * it must be accessed using the bit mask of the face using the FACE_BITMASK
     * macro - it is used to cull non-visible subchunks when rendering... */
    uint8_t visibility_graph[FID_ENTRANCES + 1];
    // Must only be used when processing the rendering BFS !
    struct cubRenderingDataBFS BFS_data;
} SubChunk;

typedef struct cubChunkFlags {
    uint8_t is_loaded       :   1;
    uint8_t is_modified     :   1;
    uint8_t unused          :   6;
} ChunkFlags;

typedef struct cubChunk {
    // X and Z coordinates of the chunk in the chunk coords system
    int32_t chunkX, chunkZ;
    uint8_t nb_subchunks;
    ChunkFlags flags;
    // List of the subchunks in this chunk
    SubChunk subchunks[MAX_SUBCHUNKS];
} Chunk;

typedef struct cubChunkCoords {
    uint8_t x   :   4;
    uint8_t z   :   4;
    uint8_t y   :   8;
} ChunkCoords;

typedef struct cubSubChunkCoords {
    uint8_t x   :   4;
    uint8_t y   :   4;
    uint8_t z   :   4;
} SubChunkCoords;

typedef struct cubChunkRenderer {
    // Position of the chunk where the camera stood during the last frame
    int32_t last_chunkX, last_chunkZ;
    Chunk* chunk_grid; // The grid representation of all the chunks loaded
    uint8_t render_distance;
    uint8_t grid_side; // = 2 * render_distance - 1
    uint16_t nb_chunks; // = grid_side * grid_side
    // The order is: Top -> Bottom -> Front -> Back -> Left -> Right
    GraphMarker* block_marker;
    Queue* SC_queue; // Subchunk Queue
    Queue* RSC_queue; // Rendering Subchunk Queue
} ChunkRenderer;

// ---------- Useful Macros ---------- //
#define CUB_SUBCHUNK_BP_UID(sc, sc_coords) (sc->blocks + sc_coords.x + \
        (sc_coords.z << 4) + (sc_coords.y << 8))
#define CUB_CHUNK_BP_UID(sc, c_coords) \
    CUB_SUBCHUNK_BP_UID(sc, chunk_coords_to_subchunk(c_coords))
#define CUB_SUBCHUNK_BLOCK_ID(sc, sc_coords) \
    (sc_coords.x + (sc_coords.z << 4) + (sc_coords.y << 8))
#define WORLD_COORDS_TO_BLOCK(w_coords) VEC3(floorf(w_coords.coords[0]),\
                                             floorf(w_coords.coords[1]),\
                                             floorf(w_coords.coords[2]))

#endif /* !CUB_CHUNK_TYPES_H */
