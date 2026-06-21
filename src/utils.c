/**
 * @file utils.c
 * @brief Implementacija pomoćnih funkcija za usporedbu, ispis i oslobađanje
 */

#include "utils.h"
#include <string.h>
#include <stdio.h>

int int_cmp(const void* a, const void* b)
{
    int val_a = *(const int*)a;
    int val_b = *(const int*)b;

    if (val_a < val_b) {
        return -1;
    }
    if (val_a > val_b) {
        return 1;
    }
    return 0;
}

int string_cmp(const void* a, const void* b)
{
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;
    return strcmp(str_a, str_b);
}

void int_print(const void* data)
{
    int val = *(const int*)data;
    printf("%d", val);
}

void string_print(const void* data)
{
    const char* str = *(const char**)data;
    printf("%s", str);
}

void generic_free(void* data)
{
    free(data);
}