#include "internal/list_doubly_internal.h"
#include "internal/list_internal.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Kreira novi čvor sa zadatim podatkom
 */
static ListNodeD* list_doubly_node_create(const void* data)
{
    ListNodeD* node = (ListNodeD*)malloc(sizeof(ListNodeD));
    if (!node) {
        return NULL;
    }
    node->data = (void*)data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

/**
 * @brief Dohvata čvor na zadanom indeksu
 */
static ListNodeD* list_doubly_node_at(ListDoubly* list, size_t index)
{
    if (!list || index >= list->count) {
        return NULL;
    }

    ListNodeD* current = list->head;
    for (size_t i = 0; i < index && current; i++) {
        current = current->next;
    }
    return current;
}

/**
 * @brief Kreira novu dvostruko povezanu listu
 */
void* list_doubly_create(void)
{
    ListDoubly* list = (ListDoubly*)malloc(sizeof(ListDoubly));
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return (void*)list;
}

/**
 * @brief Umeće element na početak liste
 */
Status list_doubly_insert_front(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    ListNodeD* node = list_doubly_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    node->next = list->head;
    node->prev = NULL;

    if (list->head) {
        list->head->prev = node;
    }
    list->head = node;

    if (list->count == 0) {
        list->tail = node;
    }

    list->count++;
    return STATUS_OK;
}

/**
 * @brief Umeće element na kraj liste
 */
Status list_doubly_insert_back(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    ListNodeD* node = list_doubly_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    node->prev = list->tail;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    }
    list->tail = node;

    if (list->count == 0) {
        list->head = node;
    }

    list->count++;
    return STATUS_OK;
}

/**
 * @brief Uklanja element sa početka liste
 */
Status list_doubly_remove_front(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    if (list->count == 0) {
        return STATUS_EMPTY;
    }

    ListNodeD* node = list->head;
    *out = node->data;

    list->head = node->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    list->count--;
    free(node);
    return STATUS_OK;
}

/**
 * @brief Uklanja element sa kraja liste
 */
Status list_doubly_remove_back(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    if (list->count == 0) {
        return STATUS_EMPTY;
    }

    ListNodeD* node = list->tail;
    *out = node->data;

    list->tail = node->prev;
    if (list->tail) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    list->count--;
    free(node);
    return STATUS_OK;
}

/**
 * @brief Dohvata element na zadanom indeksu
 */
Status list_doubly_get(void* impl, size_t index, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    if (index >= list->count) {
        return STATUS_ALLOC_ERROR;
    }

    ListNodeD* node = list_doubly_node_at(list, index);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    *out = node->data;
    return STATUS_OK;
}

/**
 * @brief Provjeri je li lista prazna
 */
int list_doubly_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const ListDoubly* list = (const ListDoubly*)impl;
    return (list->count == 0) ? 1 : 0;
}

/**
 * @brief Vraća broj elemenata
 */
size_t list_doubly_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const ListDoubly* list = (const ListDoubly*)impl;
    return list->count;
}

/**
 * @brief Ispisuje sve elemente liste
 */
Status list_doubly_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const ListDoubly* list = (const ListDoubly*)impl;
    ListNodeD* current = list->head;

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

/**
 * @brief Uništava listu i oslobađa svu memoriju
 */
Status list_doubly_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    ListDoubly* list = (ListDoubly*)impl;
    ListNodeD* current = list->head;

    while (current) {
        ListNodeD* next = current->next;
        if (ff) {
            ff(current->data);
        }
        free(current);
        current = next;
    }

    free(list);
    return STATUS_OK;
}

const ListVtable list_doubly_vtable = {
    .insert_front = list_doubly_insert_front,
    .insert_back = list_doubly_insert_back,
    .remove_front = list_doubly_remove_front,
    .remove_back = list_doubly_remove_back,
    .get = list_doubly_get,
    .is_empty = list_doubly_is_empty,
    .size = list_doubly_size,
    .print = list_doubly_print,
    .destroy = list_doubly_destroy
};
