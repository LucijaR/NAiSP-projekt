#include "internal/stack_list_internal.h"
#include "internal/stack_internal.h"
#include <stdlib.h>
#include <stdio.h>

static StackNode* stack_list_node_create(const void* data)
{
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    if (!node) {
        return NULL;
    }
    node->data = (void*)data;
    node->next = NULL;
    return node;
}

void* stack_list_create(void)
{
    StackList* stack = (StackList*)malloc(sizeof(StackList));
    if (!stack) {
        return NULL;
    }
    stack->head = NULL;
    stack->size = 0;
    return stack;
}

Status stack_list_push(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    StackList* stack = (StackList*)impl;
    StackNode* node = stack_list_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    node->next = stack->head;
    stack->head = node;
    stack->size++;
    return STATUS_OK;
}

Status stack_list_pop(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    StackList* stack = (StackList*)impl;
    if (stack->size == 0) {
        return STATUS_EMPTY;
    }

    StackNode* node = stack->head;
    *out = node->data;
    stack->head = node->next;
    stack->size--;
    free(node);
    return STATUS_OK;
}

Status stack_list_peek(const void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    const StackList* stack = (const StackList*)impl;
    if (stack->size == 0) {
        return STATUS_EMPTY;
    }

    *out = stack->head->data;
    return STATUS_OK;
}

int stack_list_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const StackList* stack = (const StackList*)impl;
    return stack->size == 0 ? 1 : 0;
}

size_t stack_list_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const StackList* stack = (const StackList*)impl;
    return stack->size;
}

Status stack_list_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const StackList* stack = (const StackList*)impl;
    StackNode* current = stack->head;
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

Status stack_list_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    StackList* stack = (StackList*)impl;
    StackNode* current = stack->head;
    while (current) {
        StackNode* next = current->next;
        if (ff) {
            ff(current->data);
        }
        free(current);
        current = next;
    }

    free(stack);
    return STATUS_OK;
}

const StackVtable stack_list_vtable = {
    .push = stack_list_push,
    .pop = stack_list_pop,
    .peek = stack_list_peek,
    .is_empty = stack_list_is_empty,
    .size = stack_list_size,
    .print = stack_list_print,
    .destroy = stack_list_destroy
};
