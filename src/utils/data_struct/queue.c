#include "data_struct/queue.h"

Queue* queue_new() {
    Queue* Q = malloc(sizeof(Queue));
    if (!Q) errx(EXIT_FAILURE, "queue_new: malloc failed!");
    Q->available_elts = list_new(sizeof(queueElement*), REALLOC_INCREMENT, 0);
    Q->head = NULL;
    Q->tail = NULL;
    return Q;
}

bool queue_is_empty(Queue* Q) {
    return Q->head == NULL;
}

void queue_enqueue(Queue* Q, void* elt) {
    queueElement* q_elt;
    if (Q->available_elts->len == 0) {
        q_elt = malloc(sizeof(queueElement));
        if (!q_elt) errx(EXIT_FAILURE, "queue_enqueue: Malloc failed!");
    } else
        list_pop(Q->available_elts, Q->available_elts->len - 1, &q_elt);
    q_elt->data = elt;
    q_elt->next = NULL;
    if (queue_is_empty(Q)) {
        Q->head = q_elt;
        Q->tail = q_elt;
    } else {
        Q->tail->next = q_elt;
        Q->tail = q_elt;
    }
}

void* queue_dequeue(Queue* Q) {
#ifdef CUB_DEBUG
    if (queue_is_empty(Q)) errx(EXIT_FAILURE, "queue_dequeue: empty queue!");
#endif
    queueElement* q_elt = Q->head;
    list_append(Q->available_elts, &q_elt);
    Q->head = Q->head->next;
    if (Q->head == NULL)
        Q->tail = NULL;
    q_elt->next = NULL;
    return q_elt->data;
}

void queue_clear(Queue* Q) {
    while (!queue_is_empty(Q))
        queue_dequeue(Q);
}

void queue_free(Queue* Q) {
    queueElement** q_elts = Q->available_elts->data;
    for (size_t i = 0; i < Q->available_elts->len; ++i)
        free(q_elts[i]);
    list_free(Q->available_elts);
    queueElement* q_elt = Q->head;
    while (q_elt) {
        queueElement* temp = q_elt;
        q_elt = q_elt->next;
        free(temp);
    }
    free(Q);
}
