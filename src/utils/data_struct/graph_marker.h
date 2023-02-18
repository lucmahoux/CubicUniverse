#ifndef CUB_UTILS_GRAPH_MARKER_H
#define CUB_UTILS_GRAPH_MARKER_H

#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GRAPH_MARK_MAX_VALUE 255
#define GRAPH_MARK_MIN_VALUE 1

typedef struct cubGraphMarker {
    uint8_t* marking_list;
    uint8_t mark_value;
    size_t nb_elements;
} GraphMarker;

GraphMarker* utils_graph_marker_new(size_t nb_elements);

void utils_graph_marker_init(GraphMarker* marker);

bool utils_graph_marker_is_marked(GraphMarker* marker, size_t index);

void utils_graph_marker_mark(GraphMarker* marker, size_t index);

void utils_graph_marker_free(GraphMarker* marker);

#endif
