#include "utils/graph_marker.h"

GraphMarker* utils_graph_marker_new(size_t nb_elements) {
    GraphMarker* marker = malloc(sizeof(GraphMarker));
    if (!marker) errx(EXIT_FAILURE, "utils_graph_marker_new: malloc failed!");
    marker->marking_list = calloc(nb_elements, sizeof(uint8_t));
    if (!marker->marking_list) {
        free(marker);
        errx(EXIT_FAILURE, "utils_graph_marker_new: malloc failed!");
    }
    marker->mark_value = 0;
    marker->nb_elements = nb_elements;
    return marker;
}

void utils_graph_marker_init(GraphMarker* marker) {
    if (marker->mark_value >= GRAPH_MARK_MAX_VALUE) {
        marker->mark_value = GRAPH_MARK_MIN_VALUE;
        memset(marker->marking_list, 0, marker->nb_elements * sizeof(uint8_t));
    } else ++marker->mark_value;
}

bool utils_graph_marker_is_marked(GraphMarker* marker, size_t index) {
    return marker->marking_list[index] == marker->mark_value;
}

void utils_graph_marker_mark(GraphMarker* marker, size_t index) {
    marker->marking_list[index] = marker->mark_value;
}

void utils_graph_marker_free(GraphMarker* marker) {
    free(marker->marking_list);
    free(marker);
}
