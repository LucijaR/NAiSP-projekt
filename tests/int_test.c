#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"

static int test_failures = 0;

/* Mala pomoćna funkcija za dinamičko kreiranje int vrijednosti. */
static int* make_int(int value)
{
    int* number = (int*)malloc(sizeof(int));
    if (number) {
        *number = value;
    }
    return number;
}

/* Provjerava da li je vraćeni Status isti kao očekivani. */
static void report_status(const char* label, Status actual, Status expected)
{
    if (actual != expected) {
        printf("[FAIL] %s: expected %d, got %d\n", label, expected, actual);
        test_failures++;
    }
}

/* Provjerava da li pokazivač stvarno pokazuje na očekivani int. */
static void report_int(const char* label, const void* actual, int expected)
{
    if (!actual) {
        printf("[FAIL] %s: got NULL\n", label);
        test_failures++;
        return;
    }

    if (*(const int*)actual != expected) {
        printf("[FAIL] %s: expected %d, got %d\n", label, expected, *(const int*)actual);
        test_failures++;
    }
}

/* Provjerava veličinu strukture nakon operacija. */
static void report_size(const char* label, size_t actual, size_t expected)
{
    if (actual != expected) {
        printf("[FAIL] %s: expected %zu, got %zu\n", label, expected, actual);
        test_failures++;
    }
}

/* Testira stog sa integerima za obje implementacije: array i linked list. */
static int run_stack_suite(StackImplType impl, const char* name)
{
    printf("\n=== STACK: %s ===\n", name);

    /* Kreiramo stog sa int helper funkcijama i free funkcijom za elemente. */
    Stack* stack = stack_create(impl, int_cmp, int_print, generic_free, 2);
    if (!stack) {
        printf("[FAIL] stack_create failed\n");
        return 1;
    }

    /* Tri vrijednosti su dovoljne da se vidi LIFO redoslijed i resize. */
    int* values[3] = { make_int(10), make_int(20), make_int(30) };
    if (!values[0] || !values[1] || !values[2]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 3; i++) {
            free(values[i]);
        }
        stack_destroy(stack);
        return 1;
    }

    report_status("stack initially empty", stack_is_empty(stack) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("stack initial size", stack_size(stack), 0);

    /* Push vrijednosti i provjeri da se veličina povećava poslije svake operacije. */
    for (size_t i = 0; i < 3; i++) {
        char label[64];
        snprintf(label, sizeof(label), "stack_push %zu", i + 1);
        report_status(label, stack_push(stack, values[i]), STATUS_OK);
        values[i] = NULL;

        snprintf(label, sizeof(label), "stack_size after push %zu", i + 1);
        report_size(label, stack_size(stack), i + 1);
    }

    report_status("stack print", stack_print(stack), STATUS_OK);

    /* Peek ne uklanja element, a pop treba vraćati obrnut redoslijed. */
    void* out = NULL;
    report_status("stack_peek", stack_peek(stack, &out), STATUS_OK);
    report_int("stack_peek value", out, 30);

    report_status("stack_pop #1", stack_pop(stack, &out), STATUS_OK);
    report_int("stack_pop #1 value", out, 30);
    free(out);

    report_status("stack_pop #2", stack_pop(stack, &out), STATUS_OK);
    report_int("stack_pop #2 value", out, 20);
    free(out);

    report_status("stack_pop #3", stack_pop(stack, &out), STATUS_OK);
    report_int("stack_pop #3 value", out, 10);
    free(out);

    report_status("stack pop on empty", stack_pop(stack, &out), STATUS_EMPTY);
    report_status("stack peek on empty", stack_peek(stack, &out), STATUS_EMPTY);
    report_size("stack final size", stack_size(stack), 0);
    report_status("stack destroy", stack_destroy(stack), STATUS_OK);

    return 0;
}

/* Testira red čekanja sa integerima za obje implementacije: array i linked list. */
static int run_queue_suite(QueueImplType impl, const char* name)
{
    printf("\n=== QUEUE: %s ===\n", name);

    /* Queue koristi iste int helper funkcije kao i stog. */
    Queue* queue = queue_create(impl, int_cmp, int_print, generic_free, 2);
    if (!queue) {
        printf("[FAIL] queue_create failed\n");
        return 1;
    }

    /* Vrijednosti su postavljene tako da se lako vidi FIFO redoslijed. */
    int* values[3] = { make_int(100), make_int(200), make_int(300) };
    if (!values[0] || !values[1] || !values[2]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 3; i++) {
            free(values[i]);
        }
        queue_destroy(queue);
        return 1;
    }

    report_status("queue initially empty", queue_is_empty(queue) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("queue initial size", queue_size(queue), 0);

    /* Enqueue treba čuvati redoslijed kako je unesen. */
    for (size_t i = 0; i < 3; i++) {
        char label[64];
        snprintf(label, sizeof(label), "queue_enqueue %zu", i + 1);
        report_status(label, queue_enqueue(queue, values[i]), STATUS_OK);
        values[i] = NULL;

        snprintf(label, sizeof(label), "queue_size after enqueue %zu", i + 1);
        report_size(label, queue_size(queue), i + 1);
    }

    report_status("queue print", queue_print(queue), STATUS_OK);

    /* Front samo čita prvi element, a dequeue ga uklanja. */
    void* out = NULL;
    report_status("queue_front", queue_front(queue, &out), STATUS_OK);
    report_int("queue_front value", out, 100);

    report_status("queue_dequeue #1", queue_dequeue(queue, &out), STATUS_OK);
    report_int("queue_dequeue #1 value", out, 100);
    free(out);

    report_status("queue_dequeue #2", queue_dequeue(queue, &out), STATUS_OK);
    report_int("queue_dequeue #2 value", out, 200);
    free(out);

    report_status("queue_dequeue #3", queue_dequeue(queue, &out), STATUS_OK);
    report_int("queue_dequeue #3 value", out, 300);
    free(out);

    report_status("queue dequeue on empty", queue_dequeue(queue, &out), STATUS_EMPTY);
    report_status("queue front on empty", queue_front(queue, &out), STATUS_EMPTY);
    report_size("queue final size", queue_size(queue), 0);
    report_status("queue destroy", queue_destroy(queue), STATUS_OK);

    return 0;
}

/* Testira listu sa integerima za singly i doubly implementaciju. */
static int run_list_suite(ListImplType impl, const char* name)
{
    printf("\n=== LIST: %s ===\n", name);

    /* Lista dobija istu int konfiguraciju kao stog i red. */
    List* list = list_create(impl, int_cmp, int_print, generic_free);
    if (!list) {
        printf("[FAIL] list_create failed\n");
        return 1;
    }

    /* Ubacujemo četiri vrijednosti da testiramo front/back i get po indeksu. */
    int* values[4] = { make_int(1), make_int(2), make_int(3), make_int(0) };
    if (!values[0] || !values[1] || !values[2] || !values[3]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 4; i++) {
            free(values[i]);
        }
        list_destroy(list);
        return 1;
    }

    report_status("list initially empty", list_is_empty(list) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("list initial size", list_size(list), 0);

    /* Najprije back inserti, pa front insert, da se vidi da redoslijed radi. */
    report_status("list_insert_back 1", list_insert_back(list, values[0]), STATUS_OK);
    values[0] = NULL;
    report_status("list_insert_back 2", list_insert_back(list, values[1]), STATUS_OK);
    values[1] = NULL;
    report_status("list_insert_back 3", list_insert_back(list, values[2]), STATUS_OK);
    values[2] = NULL;
    report_status("list_insert_front 0", list_insert_front(list, values[3]), STATUS_OK);
    values[3] = NULL;

    report_size("list size after inserts", list_size(list), 4);

    /* Provjera čitanja po indeksu za sve pozicije. */
    void* out = NULL;
    report_status("list_get 0", list_get(list, 0, &out), STATUS_OK);
    report_int("list_get 0 value", out, 0);

    report_status("list_get 1", list_get(list, 1, &out), STATUS_OK);
    report_int("list_get 1 value", out, 1);

    report_status("list_get 2", list_get(list, 2, &out), STATUS_OK);
    report_int("list_get 2 value", out, 2);

    report_status("list_get 3", list_get(list, 3, &out), STATUS_OK);
    report_int("list_get 3 value", out, 3);

    report_status("list_remove_front", list_remove_front(list, &out), STATUS_OK);
    report_int("list_remove_front value", out, 0);
    free(out);

    report_status("list_remove_back", list_remove_back(list, &out), STATUS_OK);
    report_int("list_remove_back value", out, 3);
    free(out);

    report_size("list size after removals", list_size(list), 2);
    report_status("list print", list_print(list), STATUS_OK);
    report_status("list destroy", list_destroy(list), STATUS_OK);

    return 0;
}

int main(void)
{
    printf("Pokrećem int testove za sve linearne strukture...\n");

    test_failures += run_stack_suite(STACK_IMPL_ARRAY, "array");
    test_failures += run_stack_suite(STACK_IMPL_LINKED_LIST, "linked list");

    test_failures += run_queue_suite(QUEUE_IMPL_ARRAY, "array");
    test_failures += run_queue_suite(QUEUE_IMPL_LIST, "linked list");

    test_failures += run_list_suite(LIST_IMPL_SINGLY, "singly linked");
    test_failures += run_list_suite(LIST_IMPL_DOUBLY, "doubly linked");

    if (test_failures == 0) {
        printf("\nSvi int testovi su prošli uspješno.\n");
        return EXIT_SUCCESS;
    }

    printf("\nUkupno neuspjelih provjera: %d\n", test_failures);
    return EXIT_FAILURE;
}