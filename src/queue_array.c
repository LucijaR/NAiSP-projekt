#include "internal/queue_array_internal.h"
#include "internal/queue_internal.h"
#include <stdlib.h>
#include <stdio.h>

static QueueArray* queue_array_create_impl(size_t capacity)
{
    if (capacity == 0) {
        capacity = 8;
    }

    QueueArray* queue = (QueueArray*)malloc(sizeof(QueueArray));
    if (!queue) {
        return NULL;
    }

    queue->data = (void**)malloc(sizeof(void*) * capacity);
    if (!queue->data) {
        free(queue);
        return NULL;
    }

    queue->capacity = capacity;
    queue->count = 0;
    queue->front = 0;
    queue->rear = 0;
    return queue;
}

static Status queue_array_resize(QueueArray* queue, size_t new_capacity)
{
    if (!queue || new_capacity == 0) {
        return STATUS_ALLOC_ERROR;
    }

    void** new_data = (void**)malloc(sizeof(void*) * new_capacity);
    if (!new_data) {
        return STATUS_ALLOC_ERROR;
    }

    for (size_t i = 0; i < queue->count; i++) {
        new_data[i] = queue->data[(queue->front + i) % queue->capacity];
    }

    free(queue->data);
    queue->data = new_data;
    queue->capacity = new_capacity;
    queue->front = 0;
    queue->rear = queue->count;
    return STATUS_OK;
}

void* queue_array_create(size_t capacity)
{
    return (void*)queue_array_create_impl(capacity);
}

Status queue_array_enqueue(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    QueueArray* queue = (QueueArray*)impl;
    if (queue->count == queue->capacity) {
        Status result = queue_array_resize(queue, queue->capacity * 2);
        if (result != STATUS_OK) {
            return STATUS_ALLOC_ERROR;
        }
    }

    queue->data[queue->rear] = (void*)data;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->count++;
    return STATUS_OK;
}

Status queue_array_dequeue(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    QueueArray* queue = (QueueArray*)impl;
    if (queue->count == 0) {
        return STATUS_EMPTY;
    }

    *out = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;
    return STATUS_OK;
}

Status queue_array_front(const void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    const QueueArray* queue = (const QueueArray*)impl;
    if (queue->count == 0) {
        return STATUS_EMPTY;
    }

    *out = queue->data[queue->front];
    return STATUS_OK;
}

int queue_array_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const QueueArray* queue = (const QueueArray*)impl;
    return queue->count == 0 ? 1 : 0;
}

size_t queue_array_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const QueueArray* queue = (const QueueArray*)impl;
    return queue->count;
}

Status queue_array_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const QueueArray* queue = (const QueueArray*)impl;
    for (size_t i = 0; i < queue->count; i++) {
        size_t index = (queue->front + i) % queue->capacity;
        if (pf) {
            pf(queue->data[index]);
        } else {
            printf("  <element>\n");
        }
    }
    return STATUS_OK;
}

Status queue_array_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    QueueArray* queue = (QueueArray*)impl;
    if (ff) {
        for (size_t i = 0; i < queue->count; i++) {
            size_t index = (queue->front + i) % queue->capacity;
            ff(queue->data[index]);
        }
    }

    free(queue->data);
    free(queue);
    return STATUS_OK;
}

const QueueVtable queue_array_vtable = {
    .enqueue = queue_array_enqueue,
    .dequeue = queue_array_dequeue,
    .front = queue_array_front,
    .is_empty = queue_array_is_empty,
    .size = queue_array_size,
    .print = queue_array_print,
    .destroy = queue_array_destroy
};
