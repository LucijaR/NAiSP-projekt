/**
 * @file list_singly_internal.h
 * @brief Privatni header za singly-linked list implementaciju
 * 
 * Definiše čvorne i strukturu za jednostruko povezanu listu.
 * Ovaj header se NIKAD ne uključuje u javne headere.
 * 
 * SAMO src/list_singly.c trebam uključiti ovaj header!
 */

#ifndef LIST_SINGLY_INTERNAL_H
#define LIST_SINGLY_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct ListNode
 * @brief Čvor u jednostruko povezanoj listi
 * 
 * Svaki čvor sadrži pokazivač na podatak i pokazivač na sljedeći čvor.
 */
typedef struct ListNode {
    void* data;              /**< Pokazivač na podatak */
    struct ListNode* next;   /**< Pokazivač na sljedeći čvor */
} ListNode;

/**
 * @struct ListSingly
 * @brief Struktura za jednostruko povezanu listu
 * 
 * Čuva pokazivače na prvi i posljednji čvor, te broj elemenata.
 */
typedef struct {
    ListNode* head;     /**< Pokazivač na prvi čvor (NULL ako je prazna) */
    ListNode* tail;     /**< Pokazivač na posljednji čvor (NULL ako je prazna) */
    size_t count;       /**< Broj elemenata u listi */
} ListSingly;

#endif // LIST_SINGLY_INTERNAL_H
