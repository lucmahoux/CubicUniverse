#include <criterion/criterion.h>
#include "utils/queue.h"

Test(Queue, init_free) {
    Queue* Q = queue_new();
    cr_assert(Q->head == NULL);
    cr_assert(Q->tail == NULL);
    cr_assert(Q->available_elts->len == 0);
    queue_free(Q);
}

Test(Queue, enqueue) {
    Queue* Q = queue_new();
    int values[] = { 1, 52, 6, 45, 30 };
    for (uint8_t i = 0; i < 5; ++i)
        queue_enqueue(Q, values + i);
    queueElement* q_elt = Q->head;
    for (uint8_t i = 0; i < 5; ++i) {
        cr_assert(*(int*)(q_elt->data) == values[i]);
        q_elt = q_elt->next;
    }
    cr_assert(*(int*)Q->head->data == 1);
    cr_assert(*(int*)Q->tail->data == 30);
    queue_free(Q);
}

Test(Queue, dequeue) {
    Queue* Q = queue_new();
    int values[] = { 1, 52, 6, 45, 30 };
    for (uint8_t i = 0; i < 5; ++i)
        queue_enqueue(Q, values + i);
    int* elt;
    for (uint8_t i = 0; i < 5; ++i) {
        elt = queue_dequeue(Q);
        cr_assert(Q->available_elts->len == (size_t)(i + 1));
        cr_assert(*elt == values[i]);
    }
    cr_assert(Q->head == NULL);
    cr_assert(Q->tail == NULL);
    queue_free(Q);
}

Test(Queue, is_empty) {
    Queue* Q = queue_new();
    int elt = 10;
    cr_assert(queue_is_empty(Q) == true);
    queue_enqueue(Q, &elt);
    cr_assert(queue_is_empty(Q) == false);
    queue_dequeue(Q);
    cr_assert(queue_is_empty(Q) == true);
    queue_free(Q);
}

Test(Queue, clear) {
    Queue* Q = queue_new();
    int values[] = { 1, 52, 6, 45, 30 };
    for (uint8_t i = 0; i < 5; ++i)
        queue_enqueue(Q, values + i);
    queue_clear(Q);
    cr_assert(queue_is_empty(Q) == true);
    queue_free(Q);
}
