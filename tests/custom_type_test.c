/*
Run (PowerShell):

cmake -S . -B build
cmake --build build --target custom_type_test_program
.\build\custom_type_test_program.exe

Tokom testa ne unosiš ništa ručno; program sam generiše veliki broj elemenata.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"

typedef struct Student {
    int indeks;
    char ime[32];
    float prosjek;
} Student;

static int test_failures = 0;

enum {
    BULK_TEST_COUNT = 128
};

/* Kreira primjer vlastitog tipa podataka. */
static Student* make_student(int indeks, const char* ime, float prosjek)
{
    Student* student = (Student*)malloc(sizeof(Student));
    if (!student) {
        return NULL;
    }

    student->indeks = indeks;
    snprintf(student->ime, sizeof(student->ime), "%s", ime);
    student->prosjek = prosjek;
    return student;
}

/* Uspoređuje dva studenta po indeksu. */
static int student_cmp(const void* a, const void* b)
{
    const Student* left = (const Student*)a;
    const Student* right = (const Student*)b;
    if (left->indeks < right->indeks) {
        return -1;
    }
    if (left->indeks > right->indeks) {
        return 1;
    }
    return 0;
}

/* Ispisuje jednog studenta. */
static void student_print(const void* data)
{
    const Student* student = (const Student*)data;
    printf("{indeks=%d, ime=%s, prosjek=%.2f}", student->indeks, student->ime, student->prosjek);
}

/* Provjerava status poziva. */
static void report_status(const char* label, Status actual, Status expected)
{
    if (actual != expected) {
        printf("[FAIL] %s: expected %d, got %d\n", label, expected, actual);
        test_failures++;
    }
}

/* Provjerava student podatke. */
static void report_student(const char* label, const void* actual, int indeks, const char* ime, float prosjek)
{
    if (!actual) {
        printf("[FAIL] %s: got NULL\n", label);
        test_failures++;
        return;
    }

    const Student* student = (const Student*)actual;
    if (student->indeks != indeks || strcmp(student->ime, ime) != 0 || student->prosjek != prosjek) {
        printf("[FAIL] %s: expected {%d, %s, %.2f}, got {%d, %s, %.2f}\n",
               label, indeks, ime, prosjek, student->indeks, student->ime, student->prosjek);
        test_failures++;
    }
}

/* Provjerava veličinu strukture. */
static void report_size(const char* label, size_t actual, size_t expected)
{
    if (actual != expected) {
        printf("[FAIL] %s: expected %zu, got %zu\n", label, expected, actual);
        test_failures++;
    }
}

/* Pravi naziv za generisani bulk student. */
static void make_bulk_name(size_t index, char* buffer, size_t buffer_size)
{
    snprintf(buffer, buffer_size, "Bulk-%03zu", index);
}

/* Pravi jednog studenta za bulk test. */
static Student* make_bulk_student(size_t index)
{
    char name[32];
    make_bulk_name(index, name, sizeof(name));
    return make_student(10000 + (int)index, name, 9.00f);
}

/* Task: testiranje rada s velikim brojem elemenata - stog. */
static int run_bulk_stack_suite(StackImplType impl, const char* name)
{
    printf("\n=== BULK CUSTOM TYPE / STACK: %s ===\n", name);

    Stack* stack = stack_create(impl, student_cmp, student_print, generic_free, 4);
    if (!stack) {
        printf("[FAIL] stack_create failed\n");
        return 1;
    }

    void* out = NULL;
    report_status("bulk stack initially empty", stack_is_empty(stack) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk stack initial size", stack_size(stack), 0);
    report_status("bulk stack print empty", stack_print(stack), STATUS_OK);
    report_status("bulk stack pop on empty", stack_pop(stack, &out), STATUS_EMPTY);
    report_status("bulk stack peek on empty", stack_peek(stack, &out), STATUS_EMPTY);

    Student* students[BULK_TEST_COUNT] = {0};
    for (size_t i = 0; i < BULK_TEST_COUNT; i++) {
        students[i] = make_bulk_student(i);
        if (!students[i]) {
            printf("[FAIL] bulk stack memory allocation failed\n");
            for (size_t j = 0; j < i; j++) {
                if (students[j]) {
                    free(students[j]);
                }
            }
            stack_destroy(stack);
            return 1;
        }

        char label[64];
        snprintf(label, sizeof(label), "bulk stack_push %zu", i);
        report_status(label, stack_push(stack, students[i]), STATUS_OK);
        students[i] = NULL;
    }

    report_size("bulk stack size after pushes", stack_size(stack), BULK_TEST_COUNT);
    report_status("bulk stack print", stack_print(stack), STATUS_OK);

    for (size_t remaining = BULK_TEST_COUNT; remaining > 0; remaining--) {
        size_t index = remaining - 1;
        char expected_name[32];
        char label[64];

        snprintf(label, sizeof(label), "bulk stack_pop %zu", index);
        report_status(label, stack_pop(stack, &out), STATUS_OK);
        make_bulk_name(index, expected_name, sizeof(expected_name));
        report_student("bulk stack pop value", out, 10000 + (int)index, expected_name, 9.00f);
        free(out);
    }

    report_status("bulk stack empty after pops", stack_is_empty(stack) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk stack final size", stack_size(stack), 0);
    report_status("bulk stack destroy", stack_destroy(stack), STATUS_OK);
    return 0;
}

/* Task: testiranje rada s velikim brojem elemenata - red. */
static int run_bulk_queue_suite(QueueImplType impl, const char* name)
{
    printf("\n=== BULK CUSTOM TYPE / QUEUE: %s ===\n", name);

    Queue* queue = queue_create(impl, student_cmp, student_print, generic_free, 4);
    if (!queue) {
        printf("[FAIL] queue_create failed\n");
        return 1;
    }

    void* out = NULL;
    report_status("bulk queue initially empty", queue_is_empty(queue) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk queue initial size", queue_size(queue), 0);
    report_status("bulk queue print empty", queue_print(queue), STATUS_OK);
    report_status("bulk queue dequeue on empty", queue_dequeue(queue, &out), STATUS_EMPTY);
    report_status("bulk queue front on empty", queue_front(queue, &out), STATUS_EMPTY);

    Student* students[BULK_TEST_COUNT] = {0};
    for (size_t i = 0; i < BULK_TEST_COUNT; i++) {
        students[i] = make_bulk_student(i);
        if (!students[i]) {
            printf("[FAIL] bulk queue memory allocation failed\n");
            for (size_t j = 0; j < i; j++) {
                if (students[j]) {
                    free(students[j]);
                }
            }
            queue_destroy(queue);
            return 1;
        }

        char label[64];
        snprintf(label, sizeof(label), "bulk queue_enqueue %zu", i);
        report_status(label, queue_enqueue(queue, students[i]), STATUS_OK);
        students[i] = NULL;
    }

    report_size("bulk queue size after enqueues", queue_size(queue), BULK_TEST_COUNT);
    report_status("bulk queue print", queue_print(queue), STATUS_OK);

    char expected_name[32];
    make_bulk_name(0, expected_name, sizeof(expected_name));
    report_status("bulk queue front", queue_front(queue, &out), STATUS_OK);
    report_student("bulk queue front value", out, 10000, expected_name, 9.00f);

    for (size_t index = 0; index < BULK_TEST_COUNT; index++) {
        char label[64];
        make_bulk_name(index, expected_name, sizeof(expected_name));

        snprintf(label, sizeof(label), "bulk queue_dequeue %zu", index);
        report_status(label, queue_dequeue(queue, &out), STATUS_OK);
        report_student("bulk queue dequeue value", out, 10000 + (int)index, expected_name, 9.00f);
        free(out);
    }

    report_status("bulk queue empty after dequeues", queue_is_empty(queue) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk queue final size", queue_size(queue), 0);
    report_status("bulk queue destroy", queue_destroy(queue), STATUS_OK);
    return 0;
}

/* Task: testiranje rada s velikim brojem elemenata - lista. */
static int run_bulk_list_suite(void)
{
    printf("\n=== BULK CUSTOM TYPE / LIST ===\n");

    List* list = list_create(LIST_IMPL_DOUBLY, student_cmp, student_print, generic_free);
    if (!list) {
        printf("[FAIL] list_create failed\n");
        return 1;
    }

    void* out = NULL;
    report_status("bulk list initially empty", list_is_empty(list) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk list initial size", list_size(list), 0);
    report_status("bulk list print empty", list_print(list), STATUS_OK);
    report_status("bulk list remove_front on empty", list_remove_front(list, &out), STATUS_EMPTY);
    report_status("bulk list remove_back on empty", list_remove_back(list, &out), STATUS_EMPTY);
    report_status("bulk list_get on empty", list_get(list, 0, &out), STATUS_ALLOC_ERROR);

    Student* students[BULK_TEST_COUNT] = {0};
    for (size_t i = 0; i < BULK_TEST_COUNT; i++) {
        students[i] = make_bulk_student(i);
        if (!students[i]) {
            printf("[FAIL] bulk list memory allocation failed\n");
            for (size_t j = 0; j < i; j++) {
                if (students[j]) {
                    free(students[j]);
                }
            }
            list_destroy(list);
            return 1;
        }

        char label[64];
        snprintf(label, sizeof(label), "bulk list_insert_back %zu", i);
        report_status(label, list_insert_back(list, students[i]), STATUS_OK);
        students[i] = NULL;
    }

    report_size("bulk list size after inserts", list_size(list), BULK_TEST_COUNT);
    report_status("bulk list print", list_print(list), STATUS_OK);

    char expected_name[32];
    make_bulk_name(0, expected_name, sizeof(expected_name));
    report_status("bulk list_get 0", list_get(list, 0, &out), STATUS_OK);
    report_student("bulk list_get 0 value", out, 10000, expected_name, 9.00f);

    for (size_t index = 0; index < BULK_TEST_COUNT; index++) {
        char label[64];
        make_bulk_name(index, expected_name, sizeof(expected_name));

        snprintf(label, sizeof(label), "bulk list_remove_front %zu", index);
        report_status(label, list_remove_front(list, &out), STATUS_OK);
        report_student("bulk list remove value", out, 10000 + (int)index, expected_name, 9.00f);
        free(out);
    }

    report_status("bulk list empty after removals", list_is_empty(list) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("bulk list final size", list_size(list), 0);
    report_status("bulk list destroy", list_destroy(list), STATUS_OK);
    return 0;
}

/* Task: zamjena implementacije stoga bez promjene logike. */
static int run_stack_suite(StackImplType impl, const char* name)
{
    printf("\n=== CUSTOM TYPE / STACK: %s ===\n", name);

    Stack* stack = stack_create(impl, student_cmp, student_print, generic_free, 2);
    if (!stack) {
        printf("[FAIL] stack_create failed\n");
        return 1;
    }

    /* Task: testiranje rada na praznim strukturama i rubnih slučajeva. */
    void* out = NULL;
    report_status("stack initially empty", stack_is_empty(stack) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("stack initial size", stack_size(stack), 0);
    report_status("stack print empty", stack_print(stack), STATUS_OK);
    report_status("stack pop on empty", stack_pop(stack, &out), STATUS_EMPTY);
    report_status("stack peek on empty", stack_peek(stack, &out), STATUS_EMPTY);

    Student* first = make_student(1001, "Ana", 9.75f);
    Student* second = make_student(1002, "Marko", 8.50f);
    Student* third = make_student(1003, "Iva", 9.10f);
    if (!first || !second || !third) {
        printf("[FAIL] memory allocation failed\n");
        free(first);
        free(second);
        free(third);
        stack_destroy(stack);
        return 1;
    }

    report_status("stack_push first", stack_push(stack, first), STATUS_OK);
    report_status("stack_push second", stack_push(stack, second), STATUS_OK);
    report_status("stack_push third", stack_push(stack, third), STATUS_OK);
    report_size("stack size", stack_size(stack), 3);
    report_status("stack print", stack_print(stack), STATUS_OK);

    report_status("stack_peek", stack_peek(stack, &out), STATUS_OK);
    report_student("stack_peek value", out, 1003, "Iva", 9.10f);

    report_status("stack_pop", stack_pop(stack, &out), STATUS_OK);
    report_student("stack_pop value", out, 1003, "Iva", 9.10f);
    free(out);

    report_status("stack destroy", stack_destroy(stack), STATUS_OK);
    return 0;
}

/* Task: zamjena implementacije reda bez promjene logike. */
static int run_queue_suite(QueueImplType impl, const char* name)
{
    printf("\n=== CUSTOM TYPE / QUEUE: %s ===\n", name);

    Queue* queue = queue_create(impl, student_cmp, student_print, generic_free, 2);
    if (!queue) {
        printf("[FAIL] queue_create failed\n");
        return 1;
    }

    /* Task: testiranje rada na praznim strukturama i rubnih slučajeva. */
    void* out = NULL;
    report_status("queue initially empty", queue_is_empty(queue) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("queue initial size", queue_size(queue), 0);
    report_status("queue print empty", queue_print(queue), STATUS_OK);
    report_status("queue dequeue on empty", queue_dequeue(queue, &out), STATUS_EMPTY);
    report_status("queue front on empty", queue_front(queue, &out), STATUS_EMPTY);

    Student* first = make_student(2001, "Lea", 8.90f);
    Student* second = make_student(2002, "Petar", 7.80f);
    Student* third = make_student(2003, "Sara", 9.40f);
    if (!first || !second || !third) {
        printf("[FAIL] memory allocation failed\n");
        free(first);
        free(second);
        free(third);
        queue_destroy(queue);
        return 1;
    }

    report_status("queue_enqueue first", queue_enqueue(queue, first), STATUS_OK);
    report_status("queue_enqueue second", queue_enqueue(queue, second), STATUS_OK);
    report_status("queue_enqueue third", queue_enqueue(queue, third), STATUS_OK);
    report_size("queue size", queue_size(queue), 3);
    report_status("queue print", queue_print(queue), STATUS_OK);

    report_status("queue_front", queue_front(queue, &out), STATUS_OK);
    report_student("queue_front value", out, 2001, "Lea", 8.90f);

    report_status("queue_dequeue", queue_dequeue(queue, &out), STATUS_OK);
    report_student("queue_dequeue value", out, 2001, "Lea", 8.90f);
    free(out);

    report_status("queue destroy", queue_destroy(queue), STATUS_OK);
    return 0;
}

/* Primjer definisanja i korištenja vlastitog tipa na listi. */
static int run_list_suite(void)
{
    printf("\n=== CUSTOM TYPE / LIST ===\n");

    List* list = list_create(LIST_IMPL_DOUBLY, student_cmp, student_print, generic_free);
    if (!list) {
        printf("[FAIL] list_create failed\n");
        return 1;
    }

    /* Task: testiranje rada na praznim strukturama i rubnih slučajeva. */
    void* out = NULL;
    report_status("list initially empty", list_is_empty(list) ? STATUS_OK : STATUS_EMPTY, STATUS_OK);
    report_size("list initial size", list_size(list), 0);
    report_status("list print empty", list_print(list), STATUS_OK);
    report_status("list remove_front on empty", list_remove_front(list, &out), STATUS_EMPTY);
    report_status("list remove_back on empty", list_remove_back(list, &out), STATUS_EMPTY);
    report_status("list_get on empty", list_get(list, 0, &out), STATUS_ALLOC_ERROR);

    Student* first = make_student(3001, "Nina", 9.00f);
    Student* second = make_student(3002, "Dino", 8.20f);
    Student* third = make_student(3003, "Ema", 9.60f);
    if (!first || !second || !third) {
        printf("[FAIL] memory allocation failed\n");
        free(first);
        free(second);
        free(third);
        list_destroy(list);
        return 1;
    }

    report_status("list_insert_back first", list_insert_back(list, first), STATUS_OK);
    report_status("list_insert_back second", list_insert_back(list, second), STATUS_OK);
    report_status("list_insert_front third", list_insert_front(list, third), STATUS_OK);
    report_size("list size", list_size(list), 3);
    report_status("list print", list_print(list), STATUS_OK);

    report_status("list_get 0", list_get(list, 0, &out), STATUS_OK);
    report_student("list_get 0 value", out, 3003, "Ema", 9.60f);

    report_status("list_remove_back", list_remove_back(list, &out), STATUS_OK);
    report_student("list_remove_back value", out, 3002, "Dino", 8.20f);
    free(out);

    report_status("list destroy", list_destroy(list), STATUS_OK);
    return 0;
}

int main(void)
{
    printf("Pokrećem primjer vlastitog tipa podataka...\n");

    test_failures += run_stack_suite(STACK_IMPL_ARRAY, "array");
    test_failures += run_stack_suite(STACK_IMPL_LINKED_LIST, "linked list");
    test_failures += run_queue_suite(QUEUE_IMPL_ARRAY, "array");
    test_failures += run_queue_suite(QUEUE_IMPL_LIST, "linked list");
    test_failures += run_list_suite();
    test_failures += run_bulk_stack_suite(STACK_IMPL_ARRAY, "array");
    test_failures += run_bulk_stack_suite(STACK_IMPL_LINKED_LIST, "linked list");
    test_failures += run_bulk_queue_suite(QUEUE_IMPL_ARRAY, "array");
    test_failures += run_bulk_queue_suite(QUEUE_IMPL_LIST, "linked list");
    test_failures += run_bulk_list_suite();

    if (test_failures == 0) {
        printf("\nPrimjer vlastite strukture je prošao uspješno.\n");
        return EXIT_SUCCESS;
    }

    printf("\nUkupno neuspjelih provjera: %d\n", test_failures);
    return EXIT_FAILURE;
}