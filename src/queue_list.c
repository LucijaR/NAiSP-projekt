#include "internal/queue_list_internal.h"
#include "internal/queue_internal.h"
#include <stdlib.h>
#include <stdio.h>

static QueueNode* queue_list_node_create(const void* data)
{
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        return NULL;
    }
    node->data = (void*)data;
    node->next = NULL;
    return node;
}

void* queue_list_create(void)
{
    QueueList* queue = (QueueList*)malloc(sizeof(QueueList));
    if (!queue) {
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    return queue;
}

Status queue_list_enqueue(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    QueueList* queue = (QueueList*)impl;
    QueueNode* node = queue_list_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    if (!queue->tail) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    queue->count++;
    return STATUS_OK;
}

Status queue_list_dequeue(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    QueueList* queue = (QueueList*)impl;
    if (queue->count == 0) {
        return STATUS_EMPTY;
    }

    QueueNode* node = queue->head;
    *out = node->data;
    queue->head = node->next;
    if (!queue->head) {
        queue->tail = NULL;
    }

    queue->count--;
    free(node);
    return STATUS_OK;
}

Status queue_list_front(const void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    const QueueList* queue = (const QueueList*)impl;
    if (queue->count == 0) {
        return STATUS_EMPTY;
    }

    *out = queue->head->data;
    return STATUS_OK;
}

int queue_list_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const QueueList* queue = (const QueueList*)impl;
    return queue->count == 0 ? 1 : 0;
}

size_t queue_list_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const QueueList* queue = (const QueueList*)impl;
    return queue->count;
}

Status queue_list_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const QueueList* queue = (const QueueList*)impl;
    QueueNode* current = queue->head;
    while (current) {
        if (pf) {
            pf(current->data);
        } else {
            printf("  <element>\n");
        }
        current = current->next;
    }
    return STATUS_OK;
}

Status queue_list_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    QueueList* queue = (QueueList*)impl;
    QueueNode* current = queue->head;
    while (current) {
        QueueNode* next = current->next;
        if (ff) {
            ff(current->data);
        }
        free(current);
        current = next;
    }
    free(queue);
    return STATUS_OK;
}

const QueueVtable queue_list_vtable = {
    .enqueue = queue_list_enqueue,
    .dequeue = queue_list_dequeue,
    .front = queue_list_front,
    .is_empty = queue_list_is_empty,
    .size = queue_list_size,
    .print = queue_list_print,
    .destroy = queue_list_destroy
};
