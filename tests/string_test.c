#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"

static int test_failures = 0;

/* Dinamički pravi kopiju teksta. */
static char* make_string_copy(const char* text)
{
    size_t length = strlen(text) + 1;
    char* copy = (char*)malloc(length);
    if (copy) {
        memcpy(copy, text, length);
    }
    return copy;
}

/* Pravi wrapper nad stringom jer string helperi rade sa char**. */
static char** make_string_entry(const char* text)
{
    char** entry = (char**)malloc(sizeof(char*));
    if (!entry) {
        return NULL;
    }

    entry[0] = make_string_copy(text);
    if (!entry[0]) {
        free(entry);
        return NULL;
    }

    return entry;
}

/* Oslobađa i wrapper i samu string kopiju. */
static void free_string_entry(void* data)
{
    if (!data) {
        return;
    }

    char** entry = (char**)data;
    free(entry[0]);
    free(entry);
}

/* Provjerava da li je vraćeni Status isti kao očekivani. */
static void report_status(const char* label, Status actual, Status expected)
{
    if (actual != expected) {
        printf("[FAIL] %s: expected %d, got %d\n", label, expected, actual);
        test_failures++;
    }
}

/* Provjerava da li pokazivač stvarno pokazuje na očekivani string. */
static void report_string(const char* label, const void* actual, const char* expected)
{
    if (!actual) {
        printf("[FAIL] %s: got NULL\n", label);
        test_failures++;
        return;
    }

    const char* actual_text = *(const char* const*)actual;
    if (strcmp(actual_text, expected) != 0) {
        printf("[FAIL] %s: expected %s, got %s\n", label, expected, actual_text);
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

/* Testira stog sa stringovima za obje implementacije: array i linked list. */
static int run_stack_suite(StackImplType impl, const char* name)
{
    printf("\n=== STACK STRING: %s ===\n", name);

    Stack* stack = stack_create(impl, string_cmp, string_print, free_string_entry, 2);
    if (!stack) {
        printf("[FAIL] stack_create failed\n");
        return 1;
    }

    char** values[3] = { make_string_entry("alpha"), make_string_entry("beta"), make_string_entry("gamma") };
    if (!values[0] || !values[1] || !values[2]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 3; i++) {
            free_string_entry(values[i]);
        }
        stack_destroy(stack);
        return 1;
    }

    report_status("stack initially empty", stack_is_empty(stack) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("stack initial size", stack_size(stack), 0);

    for (size_t i = 0; i < 3; i++) {
        char label[64];
        snprintf(label, sizeof(label), "stack_push %zu", i + 1);
        report_status(label, stack_push(stack, values[i]), STATUS_OK);
        values[i] = NULL;

        snprintf(label, sizeof(label), "stack_size after push %zu", i + 1);
        report_size(label, stack_size(stack), i + 1);
    }

    report_status("stack print", stack_print(stack), STATUS_OK);

    void* out = NULL;
    report_status("stack_peek", stack_peek(stack, &out), STATUS_OK);
    report_string("stack_peek value", out, "gamma");

    report_status("stack_pop #1", stack_pop(stack, &out), STATUS_OK);
    report_string("stack_pop #1 value", out, "gamma");
    free_string_entry(out);

    report_status("stack_pop #2", stack_pop(stack, &out), STATUS_OK);
    report_string("stack_pop #2 value", out, "beta");
    free_string_entry(out);

    report_status("stack_pop #3", stack_pop(stack, &out), STATUS_OK);
    report_string("stack_pop #3 value", out, "alpha");
    free_string_entry(out);

    report_status("stack pop on empty", stack_pop(stack, &out), STATUS_EMPTY);
    report_status("stack peek on empty", stack_peek(stack, &out), STATUS_EMPTY);
    report_size("stack final size", stack_size(stack), 0);
    report_status("stack destroy", stack_destroy(stack), STATUS_OK);

    return 0;
}

/* Testira red čekanja sa stringovima za obje implementacije: array i linked list. */
static int run_queue_suite(QueueImplType impl, const char* name)
{
    printf("\n=== QUEUE STRING: %s ===\n", name);

    Queue* queue = queue_create(impl, string_cmp, string_print, free_string_entry, 2);
    if (!queue) {
        printf("[FAIL] queue_create failed\n");
        return 1;
    }

    char** values[3] = { make_string_entry("red"), make_string_entry("green"), make_string_entry("blue") };
    if (!values[0] || !values[1] || !values[2]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 3; i++) {
            free_string_entry(values[i]);
        }
        queue_destroy(queue);
        return 1;
    }

    report_status("queue initially empty", queue_is_empty(queue) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("queue initial size", queue_size(queue), 0);

    for (size_t i = 0; i < 3; i++) {
        char label[64];
        snprintf(label, sizeof(label), "queue_enqueue %zu", i + 1);
        report_status(label, queue_enqueue(queue, values[i]), STATUS_OK);
        values[i] = NULL;

        snprintf(label, sizeof(label), "queue_size after enqueue %zu", i + 1);
        report_size(label, queue_size(queue), i + 1);
    }

    report_status("queue print", queue_print(queue), STATUS_OK);

    void* out = NULL;
    report_status("queue_front", queue_front(queue, &out), STATUS_OK);
    report_string("queue_front value", out, "red");

    report_status("queue_dequeue #1", queue_dequeue(queue, &out), STATUS_OK);
    report_string("queue_dequeue #1 value", out, "red");
    free_string_entry(out);

    report_status("queue_dequeue #2", queue_dequeue(queue, &out), STATUS_OK);
    report_string("queue_dequeue #2 value", out, "green");
    free_string_entry(out);

    report_status("queue_dequeue #3", queue_dequeue(queue, &out), STATUS_OK);
    report_string("queue_dequeue #3 value", out, "blue");
    free_string_entry(out);

    report_status("queue dequeue on empty", queue_dequeue(queue, &out), STATUS_EMPTY);
    report_status("queue front on empty", queue_front(queue, &out), STATUS_EMPTY);
    report_size("queue final size", queue_size(queue), 0);
    report_status("queue destroy", queue_destroy(queue), STATUS_OK);

    return 0;
}

/* Testira listu sa stringovima za singly i doubly implementaciju. */
static int run_list_suite(ListImplType impl, const char* name)
{
    printf("\n=== LIST STRING: %s ===\n", name);

    List* list = list_create(impl, string_cmp, string_print, free_string_entry);
    if (!list) {
        printf("[FAIL] list_create failed\n");
        return 1;
    }

    char** values[4] = { make_string_entry("zero"), make_string_entry("one"), make_string_entry("two"), make_string_entry("front") };
    if (!values[0] || !values[1] || !values[2] || !values[3]) {
        printf("[FAIL] memory allocation failed\n");
        for (size_t i = 0; i < 4; i++) {
            free_string_entry(values[i]);
        }
        list_destroy(list);
        return 1;
    }

    report_status("list initially empty", list_is_empty(list) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("list initial size", list_size(list), 0);

    report_status("list_insert_back 1", list_insert_back(list, values[0]), STATUS_OK);
    values[0] = NULL;
    report_status("list_insert_back 2", list_insert_back(list, values[1]), STATUS_OK);
    values[1] = NULL;
    report_status("list_insert_back 3", list_insert_back(list, values[2]), STATUS_OK);
    values[2] = NULL;
    report_status("list_insert_front 0", list_insert_front(list, values[3]), STATUS_OK);
    values[3] = NULL;

    report_size("list size after inserts", list_size(list), 4);

    void* out = NULL;
    report_status("list_get 0", list_get(list, 0, &out), STATUS_OK);
    report_string("list_get 0 value", out, "front");

    report_status("list_get 1", list_get(list, 1, &out), STATUS_OK);
    report_string("list_get 1 value", out, "zero");

    report_status("list_get 2", list_get(list, 2, &out), STATUS_OK);
    report_string("list_get 2 value", out, "one");

    report_status("list_get 3", list_get(list, 3, &out), STATUS_OK);
    report_string("list_get 3 value", out, "two");

    report_status("list_remove_front", list_remove_front(list, &out), STATUS_OK);
    report_string("list_remove_front value", out, "front");
    free_string_entry(out);

    report_status("list_remove_back", list_remove_back(list, &out), STATUS_OK);
    report_string("list_remove_back value", out, "two");
    free_string_entry(out);

    report_size("list size after removals", list_size(list), 2);
    report_status("list print", list_print(list), STATUS_OK);
    report_status("list destroy", list_destroy(list), STATUS_OK);

    return 0;
}

int main(void)
{
    printf("Pokrećem string testove za sve linearne strukture...\n");

    test_failures += run_stack_suite(STACK_IMPL_ARRAY, "array");
    test_failures += run_stack_suite(STACK_IMPL_LINKED_LIST, "linked list");

    test_failures += run_queue_suite(QUEUE_IMPL_ARRAY, "array");
    test_failures += run_queue_suite(QUEUE_IMPL_LIST, "linked list");

    test_failures += run_list_suite(LIST_IMPL_SINGLY, "singly linked");
    test_failures += run_list_suite(LIST_IMPL_DOUBLY, "doubly linked");

    if (test_failures == 0) {
        printf("\nSvi string testovi su prošli uspješno.\n");
        return EXIT_SUCCESS;
    }

    printf("\nUkupno neuspjelih provjera: %d\n", test_failures);
    return EXIT_FAILURE;
}