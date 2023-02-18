#ifndef CUB_UTILS_QUEUE_H
#define CUB_UTILS_QUEUE_H

#include <stdbool.h>
#include "data_struct/list.h"

typedef struct cubQueueElement {
    struct cubQueueElement* next;
    void* data;
} queueElement;

typedef struct cubQueue {
    List* available_elts; // List of queueElement*
    queueElement* head;
    queueElement* tail;
} Queue;

Queue* queue_new();

bool queue_is_empty(Queue* Q);

void queue_enqueue(Queue* Q, void* elt);

void* queue_dequeue(Queue* Q);

void queue_clear(Queue* Q);

void queue_free(Queue* Q);

#endif
