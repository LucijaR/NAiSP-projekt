/**
 * @file list.c
 * @brief Dispatcher sloj za listu
 * 
 * Ovaj modul implementira sve javne funkcije iz list.h tako što delegira
 * kontrolu odgovarajućim funkcijama iz vtable-a. Na taj način, isti API
 * može raditi sa bilo koje implementacijom (singly-linked, doubly-linked).
 * 
 * Funkcija list_create() je namenjena da bude proširena sa switch-om
 * koja će inicijalizirati odgovarajuću implementaciju na osnovu ListImplType.
 */

#include "list.h"
#include "internal/list_internal.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Kreira novu listu sa zadanom implementacijom
 * 
 * Alocira strukturu List i delegira kreiranje stvarne implementacije
 * odgovarajućoj funkciji (npr. list_singly_create ili list_doubly_create).
 * 
 * TODO: Ova funkcija se mora proširiti sa switch-om:
 * 
 * switch (impl) {
 *     case LIST_IMPL_SINGLY:
 *         // Pozvati list_singly_create() koja vraća impl pointer
 *         // i popuniti vt sa odgovarajućim vtable
 *         break;
 *     case LIST_IMPL_DOUBLY:
 *         // Pozvati list_doubly_create() koja vraća impl pointer
 *         // i popuniti vt sa odgovarajućim vtable
 *         break;
 *     default:
 *         free(list);
 *         return NULL;
 * }
 */
List* list_create(ListImplType impl, cmp_fn cmp, print_fn print,
                  free_fn destructor)
{
    List* list = (List*)malloc(sizeof(List));
    if (!list) {
        return NULL;
    }

    list->cmp = cmp;
    list->print = print;
    list->destructor = destructor;
    list->count = 0;
    list->impl = NULL;
    list->vt = NULL;

    /* TODO: Implementiraj switch po impl kako je opisano gore */
    /* Za sada, vratimo gresku */
    fprintf(stderr, "list_create: TODO - Implementiraj kreiranje za tip %d\n", impl);
    free(list);
    return NULL;
}

/**
 * @brief Uništava listu i oslobađa svu memoriju
 */
Status list_destroy(List* list)
{
    if (!list) {
        return STATUS_NULL_ARG;
    }

    if (!list->impl || !list->vt) {
        free(list);
        return STATUS_OK;
    }

    Status result = list->vt->destroy(list->impl, list->destructor);
    free(list);
    return result;
}

/**
 * @brief Dodaje element na početak liste
 */
Status list_insert_front(List* list, const void* data)
{
    if (!list || !data) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = list->vt->insert_front(list->impl, data);
    if (result == STATUS_OK) {
        list->count++;
    }
    return result;
}

/**
 * @brief Dodaje element na kraj liste
 */
Status list_insert_back(List* list, const void* data)
{
    if (!list || !data) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = list->vt->insert_back(list->impl, data);
    if (result == STATUS_OK) {
        list->count++;
    }
    return result;
}

/**
 * @brief Uklanja element sa početka liste
 */
Status list_remove_front(List* list, void** out)
{
    if (!list || !out) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = list->vt->remove_front(list->impl, out);
    if (result == STATUS_OK && list->count > 0) {
        list->count--;
    }
    return result;
}

/**
 * @brief Uklanja element sa kraja liste
 */
Status list_remove_back(List* list, void** out)
{
    if (!list || !out) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        return STATUS_ALLOC_ERROR;
    }

    Status result = list->vt->remove_back(list->impl, out);
    if (result == STATUS_OK && list->count > 0) {
        list->count--;
    }
    return result;
}

/**
 * @brief Dohvaća element na zadanom indeksu
 */
Status list_get(List* list, size_t index, void** out)
{
    if (!list || !out) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        return STATUS_ALLOC_ERROR;
    }

    if (index >= list->count) {
        return STATUS_ALLOC_ERROR;  /* Index out of bounds */
    }

    return list->vt->get(list->impl, index, out);
}

/**
 * @brief Provjeri je li lista prazna
 */
int list_is_empty(const List* list)
{
    if (!list || !list->vt || !list->impl) {
        return 1;
    }

    return list->vt->is_empty(list->impl);
}

/**
 * @brief Vraća broj elemenata u listi
 */
size_t list_size(const List* list)
{
    if (!list || !list->vt || !list->impl) {
        return 0;
    }

    return list->vt->size(list->impl);
}

/**
 * @brief Ispisuje sve elemente liste
 */
Status list_print(const List* list)
{
    if (!list) {
        return STATUS_NULL_ARG;
    }

    if (!list->vt || !list->impl) {
        printf("[EMPTY LIST]\n");
        return STATUS_OK;
    }

    printf("List (size: %zu) [\n", list->count);
    Status result = list->vt->print(list->impl, list->print);
    printf("]\n");

    return result;
}
