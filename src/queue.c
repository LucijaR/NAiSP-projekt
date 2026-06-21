/**
 * @file queue.c
 * @brief Dispatcher sloj za red čekanja
 * 
 * Ovaj modul implementira sve javne funkcije iz queue.h tako što delegira
 * kontrolu odgovarajućim funkcijama iz vtable-a. Na taj način, isti API
 * može raditi sa bilo koje implementacijom (array-based, linked-list-based).
 * 
 * Funkcija queue_create() je namenjena da bude proširena sa switch-om
 * koja će inicijalizirati odgovarajuću implementaciju na osnovu QueueImplType.
 */

#include "queue.h"
#include "internal/queue_internal.h"
#include <stdlib.h>
#include <stdio.h>

extern const QueueVtable queue_array_vtable;

extern void* queue_array_create(size_t capacity);

extern const QueueVtable queue_list_vtable;
extern void* queue_list_create(void);

/**
 * @brief Kreira novi red čekanja sa zadanom implementacijom
 * 
 * Alocira strukturu Queue i delegira kreiranje stvarne implementacije
 * odgovarajućoj funkciji (npr. queue_array_create ili queue_list_create).
 */
Queue* queue_create(QueueImplType impl, cmp_fn cmp, print_fn print,
                    free_fn destructor, size_t capacity)
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        return NULL;
    }

    queue->cmp = cmp;
    queue->print = print;
    queue->destructor = destructor;
    queue->count = 0;
    queue->impl = NULL;
    queue->vt = NULL;

        switch (impl) {
        case QUEUE_IMPL_ARRAY:
            queue->impl = queue_array_create(capacity);
            if (!queue->impl) {
                free(queue);
                return NULL;
            }
            queue->vt = &queue_array_vtable;
            break;

        case QUEUE_IMPL_LIST:
            queue->impl = queue_list_create();
            if (!queue->impl) {
                free(queue);
                return NULL;
            }
            queue->vt = &queue_list_vtable;
            break;

        default:
            fprintf(stderr, "queue_create: Nepoznat tip implementacije: %d\n", impl);
            free(queue);
            return NULL;
    }
    return queue;
}

/**
 * @brief Uništava red čekanja i oslobađa svu memoriju
 */
Status queue_destroy(Queue* queue)
{
    if (!queue) {
        return STATUS_NULL_ARG;
    }

    if (!queue->impl || !queue->vt) {
        free(queue);
        return STATUS_OK;
    }

    Status result = queue->vt->destroy(queue->impl, queue->destructor);
    free(queue);
    return result;
}

/**
 * @brief Dodaje element na kraj reda čekanja
 */
Status queue_enqueue(Queue* queue, const void* data)
{
    if (!queue || !data) {
        return STATUS_NULL_ARG;
    }

    if (!queue->vt || !queue->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = queue->vt->enqueue(queue->impl, data);
    if (result == STATUS_OK) {
        queue->count++;
    }
    return result;
}

/**
 * @brief Uklanja i vraća element sa početka reda čekanja
 */
Status queue_dequeue(Queue* queue, void** out)
{
    if (!queue || !out) {
        return STATUS_NULL_ARG;
    }

    if (!queue->vt || !queue->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = queue->vt->dequeue(queue->impl, out);
    if (result == STATUS_OK && queue->count > 0) {
        queue->count--;
    }
    return result;
}

/**
 * @brief Vraća element sa početka reda čekanja bez uklanjanja
 */
Status queue_front(Queue* queue, void** out)
{
    if (!queue || !out) {
        return STATUS_NULL_ARG;
    }

    if (!queue->vt || !queue->impl) {
        return STATUS_ALLOC_ERROR;
    }

    return queue->vt->front(queue->impl, out);
}

/**
 * @brief Provjeri je li red čekanja prazan
 */
int queue_is_empty(const Queue* queue)
{
    if (!queue || !queue->vt || !queue->impl) {
        return 1;
    }

    return queue->vt->is_empty(queue->impl);
}

/**
 * @brief Vraća broj elemenata u redu čekanja
 */
size_t queue_size(const Queue* queue)
{
    if (!queue || !queue->vt || !queue->impl) {
        return 0;
    }

    return queue->vt->size(queue->impl);
}

/**
 * @brief Ispisuje sve elemente reda čekanja
 */
Status queue_print(const Queue* queue)
{
    if (!queue) {
        return STATUS_NULL_ARG;
    }

    if (!queue->vt || !queue->impl) {
        printf("[EMPTY QUEUE]\n");
        return STATUS_OK;
    }

    printf("Queue (size: %zu) {\n", queue->count);
    Status result = queue->vt->print(queue->impl, queue->print);
    printf("}\n");

    return result;
}
