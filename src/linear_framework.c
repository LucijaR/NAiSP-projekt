#include "linear_framework.h"
#include <stdio.h>
#include <stdlib.h>

struct LinearStructure {
    int tajni_id;
};

LinearStruct create_test_stack(void) {
    LinearStruct ls = malloc(sizeof(struct LinearStructure));
    if (ls) {
        ls->tajni_id = 42; 
    }
    return ls;
}

void pozdravi_iz_frameworka(LinearStruct ls) {
    if (ls) {
        printf("Pozdrav iz skrivenog frameworka! Moj tajni ID je: %d\n", ls->tajni_id);
    }
}

void free_test_stack(LinearStruct ls) {
    free(ls);
}