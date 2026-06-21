#include "internal/stack_array_internal.h"
#include "internal/stack_internal.h"
#include <stdlib.h>
#include <stdio.h>

static StackArray* stack_array_create_impl(size_t capacity)
{
    if (capacity == 0) {
        return NULL;
    }

    StackArray* stack = (StackArray*)malloc(sizeof(StackArray));
    if (!stack) {
        return NULL;
    }

    stack->data = (void**)malloc(sizeof(void*) * capacity);
    if (!stack->data) {
        free(stack);
        return NULL;
    }

    stack->size = 0;
    stack->capacity = capacity;
    return stack;
}

static Status stack_array_resize(StackArray* stack, size_t new_capacity)
{
    if (!stack || new_capacity == 0) {
        return STATUS_ALLOC_ERROR;
    }

    void** new_data = (void**)realloc(stack->data, sizeof(void*) * new_capacity);
    if (!new_data) {
        return STATUS_ALLOC_ERROR;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
    return STATUS_OK;
}

void* stack_array_create(size_t capacity)
{
    if (capacity == 0) {
        capacity = 8;
    }
    return (void*)stack_array_create_impl(capacity);
}

Status stack_array_push(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    StackArray* stack = (StackArray*)impl;
    if (stack->size == stack->capacity) {
        Status result = stack_array_resize(stack, stack->capacity * 2);
        if (result != STATUS_OK) {
            return STATUS_ALLOC_ERROR;
        }
    }

    stack->data[stack->size++] = (void*)data;
    return STATUS_OK;
}

Status stack_array_pop(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    StackArray* stack = (StackArray*)impl;
    if (stack->size == 0) {
        return STATUS_EMPTY;
    }

    *out = stack->data[--stack->size];
    return STATUS_OK;
}

Status stack_array_peek(const void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    const StackArray* stack = (const StackArray*)impl;
    if (stack->size == 0) {
        return STATUS_EMPTY;
    }

    *out = stack->data[stack->size - 1];
    return STATUS_OK;
}

int stack_array_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const StackArray* stack = (const StackArray*)impl;
    return stack->size == 0 ? 1 : 0;
}

size_t stack_array_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const StackArray* stack = (const StackArray*)impl;
    return stack->size;
}

Status stack_array_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const StackArray* stack = (const StackArray*)impl;
    for (size_t i = stack->size; i > 0; i--) {
        if (pf) {
            pf(stack->data[i - 1]);
        } else {
            printf("  <element>\n");
        }
    }
    return STATUS_OK;
}

Status stack_array_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    StackArray* stack = (StackArray*)impl;
    if (ff) {
        for (size_t i = 0; i < stack->size; i++) {
            ff(stack->data[i]);
        }
    }

    free(stack->data);
    free(stack);
    return STATUS_OK;
}

const StackVtable stack_array_vtable = {
    .push = stack_array_push,
    .pop = stack_array_pop,
    .peek = stack_array_peek,
    .is_empty = stack_array_is_empty,
    .size = stack_array_size,
    .print = stack_array_print,
    .destroy = stack_array_destroy
};
