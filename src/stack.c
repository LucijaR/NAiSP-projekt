/**
 * @file stack.c
 * @brief Dispatcher sloj za stogu
 * 
 * Ovaj modul implementira sve javne funkcije iz stack.h tako što delegira
 * kontrolu odgovarajućim funkcijama iz vtable-a. Na taj način, isti API
 * može raditi sa bilo koje implementacijom (array-based, linked-list-based).
 * 
 * Funkcija stack_create() je namenjena da bude proširena sa switch-om
 * koja će inicijalizirati odgovarajuću implementaciju na osnovu StackImplType.
 */

#include "stack.h"
#include "internal/stack_internal.h"
#include <stdlib.h>
#include <stdio.h>

extern const StackVtable stack_array_vtable;
extern const StackVtable stack_list_vtable;

extern void* stack_array_create(size_t capacity);
extern void* stack_list_create(void);

/**
 * @brief Kreira novi stog sa zadanom implementacijom
 * 
 * Alocira strukturu Stack i delegira kreiranje stvarne implementacije
 * odgovarajućoj funkciji (npr. stack_array_create ili stack_list_create).
 * 
 * TODO: Ova funkcija se mora proširiti sa switch-om:
 * 
 * switch (impl) {
 *     case STACK_IMPL_ARRAY:
 *         // Pozvati stack_array_create() koja vraća impl pointer
 *         // i popuniti vt sa odgovarajućim vtable
 *         break;
 *     case STACK_IMPL_LINKED_LIST:
 *         // Pozvati stack_list_create() koja vraća impl pointer
 *         // i popuniti vt sa odgovarajućim vtable
 *         break;
 *     default:
 *         free(stack);
 *         return NULL;
 * }
 */
Stack* stack_create(StackImplType impl, cmp_fn cmp, print_fn print,
                    free_fn destructor, size_t capacity)
{
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        return NULL;
    }

    stack->cmp = cmp;
    stack->print = print;
    stack->destructor = destructor;
    stack->count = 0;
    stack->impl = NULL;
    stack->vt = NULL;

    switch (impl) {
        case STACK_IMPL_ARRAY:
            stack->impl = stack_array_create(capacity);
            if (!stack->impl) {
                free(stack);
                return NULL;
            }
            stack->vt = &stack_array_vtable;
            break;

        case STACK_IMPL_LINKED_LIST:
            stack->impl = stack_list_create();
            if (!stack->impl) {
                free(stack);
                return NULL;
            }
            stack->vt = &stack_list_vtable;
            break;

        default:
            fprintf(stderr, "stack_create: Nepoznat tip implementacije: %d\n", impl);
            free(stack);
            return NULL;
    }

    return stack;
}

/**
 * @brief Uništava stogu i oslobađa svu memoriju
 */
Status stack_destroy(Stack* stack)
{
    if (!stack) {
        return STATUS_NULL_ARG;
    }

    if (!stack->impl || !stack->vt) {
        free(stack);
        return STATUS_OK;
    }

    Status result = stack->vt->destroy(stack->impl, stack->destructor);
    free(stack);
    return result;
}

/**
 * @brief Dodaje element na vrh stoga
 */
Status stack_push(Stack* stack, const void* data)
{
    if (!stack || !data) {
        return STATUS_NULL_ARG;
    }

    if (!stack->vt || !stack->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = stack->vt->push(stack->impl, data);
    if (result == STATUS_OK) {
        stack->count++;
    }
    return result;
}

/**
 * @brief Uklanja i vraća element sa vrha stoga
 */
Status stack_pop(Stack* stack, void** out)
{
    if (!stack || !out) {
        return STATUS_NULL_ARG;
    }

    if (!stack->vt || !stack->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = stack->vt->pop(stack->impl, out);
    if (result == STATUS_OK && stack->count > 0) {
        stack->count--;
    }
    return result;
}

/**
 * @brief Vraća element sa vrha stoga bez uklanjanja
 */
Status stack_peek(Stack* stack, void** out)
{
    if (!stack || !out) {
        return STATUS_NULL_ARG;
    }

    if (!stack->vt || !stack->impl) {
        return STATUS_ALLOC_ERROR;
    }

    return stack->vt->peek(stack->impl, out);
}

/**
 * @brief Provjeri je li stog prazna
 */
int stack_is_empty(const Stack* stack)
{
    if (!stack || !stack->vt || !stack->impl) {
        return 1;
    }

    return stack->vt->is_empty(stack->impl);
}

/**
 * @brief Vraća broj elemenata u stogi
 */
size_t stack_size(const Stack* stack)
{
    if (!stack || !stack->vt || !stack->impl) {
        return 0;
    }

    return stack->vt->size(stack->impl);
}

/**
 * @brief Ispisuje sve elemente stoge
 */
Status stack_print(const Stack* stack)
{
    if (!stack) {
        return STATUS_NULL_ARG;
    }

    if (!stack->vt || !stack->impl) {
        printf("[EMPTY STACK]\n");
        return STATUS_OK;
    }

    printf("Stack (size: %zu) {\n", stack->count);
    Status result = stack->vt->print(stack->impl, stack->print);
    printf("}\n");

    return result;
}
