/**
 * @file list_singly.c
 * @brief Implementacija jednostruko povezane liste
 * 
 * Ova datoteka implementira sve funkcije iz ListVtable-a za singly-linked list.
 * Svaka operacija je optimizovana za linearnu strukturu sa jednosmernim pristupom.
 */

#include "internal/list_singly_internal.h"
#include "internal/list_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ===== Pomoćne funkcije ===== */

/**
 * @brief Kreira novi čvor sa zadatim podatkom
 * 
 * @param data Pokazivač na podatak
 * @return Pokazivač na novi čvor, ili NULL ako alokacija nije uspješna
 */
static ListNode* list_node_create(const void* data)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) {
        return NULL;
    }
    node->data = (void*)data;
    node->next = NULL;
    return node;
}

/**
 * @brief Dohvata čvor na zadanom indeksu
 * 
 * @param list Pokazivač na ListSingly strukturu
 * @param index Indeks (0-based)
 * @return Pokazivač na čvor, ili NULL ako index izlazi van granica
 */
static ListNode* list_node_at(ListSingly* list, size_t index)
{
    if (!list || index >= list->count) {
        return NULL;
    }

    ListNode* current = list->head;
    for (size_t i = 0; i < index && current; i++) {
        current = current->next;
    }
    return current;
}

/* ===== API funkcije koje odgovaraju ListVtable-u ===== */

/**
 * @brief Kreira novu singly-linked listu
 */
void* list_singly_create(void)
{
    ListSingly* list = (ListSingly*)malloc(sizeof(ListSingly));
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
Status list_singly_insert_front(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;
    ListNode* node = list_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    node->next = list->head;
    list->head = node;

    /* Ako je lista bila prazna, novi čvor je i head i tail */
    if (list->count == 0) {
        list->tail = node;
    }

    list->count++;
    return STATUS_OK;
}

/**
 * @brief Umeće element na kraj liste
 */
Status list_singly_insert_back(void* impl, const void* data)
{
    if (!impl || !data) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;
    ListNode* node = list_node_create(data);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    if (list->count == 0) {
        /* Lista je prazna */
        list->head = node;
        list->tail = node;
    } else {
        /* Prikjači na tail */
        list->tail->next = node;
        list->tail = node;
    }

    list->count++;
    return STATUS_OK;
}

/**
 * @brief Uklanja element sa početka liste
 */
Status list_singly_remove_front(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;

    if (list->count == 0) {
        return STATUS_EMPTY;
    }

    ListNode* node = list->head;
    *out = node->data;

    list->head = node->next;
    list->count--;

    /* Ako je lista postala prazna, resetuj tail */
    if (list->count == 0) {
        list->tail = NULL;
    }

    free(node);
    return STATUS_OK;
}

/**
 * @brief Uklanja element sa kraja liste
 * 
 * NAPOMENA: Za singly-linked listu ovo je O(n) jer trebam
 * proći do pretposlednjeg čvora da vidim šta je novi tail.
 */
Status list_singly_remove_back(void* impl, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;

    if (list->count == 0) {
        return STATUS_EMPTY;
    }

    if (list->count == 1) {
        /* Samo jedan element */
        ListNode* node = list->head;
        *out = node->data;
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
        free(node);
        return STATUS_OK;
    }

    /* Pronađi pretposlednji čvor */
    ListNode* current = list->head;
    while (current->next != list->tail) {
        current = current->next;
    }

    ListNode* node = list->tail;
    *out = node->data;
    current->next = NULL;
    list->tail = current;
    list->count--;

    free(node);
    return STATUS_OK;
}

/**
 * @brief Dohvaća element na zadanom indeksu
 */
Status list_singly_get(void* impl, size_t index, void** out)
{
    if (!impl || !out) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;

    if (index >= list->count) {
        return STATUS_ALLOC_ERROR;  /* Index out of bounds */
    }

    ListNode* node = list_node_at(list, index);
    if (!node) {
        return STATUS_ALLOC_ERROR;
    }

    *out = node->data;
    return STATUS_OK;
}

/**
 * @brief Provjeri je li lista prazna
 */
int list_singly_is_empty(const void* impl)
{
    if (!impl) {
        return 1;
    }

    const ListSingly* list = (const ListSingly*)impl;
    return (list->count == 0) ? 1 : 0;
}

/**
 * @brief Vraća broj elemenata
 */
size_t list_singly_size(const void* impl)
{
    if (!impl) {
        return 0;
    }

    const ListSingly* list = (const ListSingly*)impl;
    return list->count;
}

/**
 * @brief Ispisuje sve elemente liste
 */
Status list_singly_print(const void* impl, print_fn pf)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    const ListSingly* list = (const ListSingly*)impl;
    ListNode* current = list->head;

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
Status list_singly_destroy(void* impl, free_fn ff)
{
    if (!impl) {
        return STATUS_NULL_ARG;
    }

    ListSingly* list = (ListSingly*)impl;
    ListNode* current = list->head;

    while (current) {
        ListNode* next = current->next;
        
        /* Oslobodi podatak ako je destructor proslijeđen */
        if (ff) {
            ff(current->data);
        }
        
        free(current);
        current = next;
    }

    free(list);
    return STATUS_OK;
}

/* ===== Vtable za singly-linked listu ===== */

/**
 * @brief Vtable sa svim funkcijama za singly-linked listu
 * 
 * Ova tablica se prosleđuje kao deo dispatcher strukture (List)
 * kada je korisnik odabrao LIST_IMPL_SINGLY.
 */
const ListVtable list_singly_vtable = {
    .insert_front = list_singly_insert_front,
    .insert_back = list_singly_insert_back,
    .remove_front = list_singly_remove_front,
    .remove_back = list_singly_remove_back,
    .get = list_singly_get,
    .is_empty = list_singly_is_empty,
    .size = list_singly_size,
    .print = list_singly_print,
    .destroy = list_singly_destroy
};
