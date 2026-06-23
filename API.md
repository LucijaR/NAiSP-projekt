# API Dokumentacija - Generički Framework za Linearne Strukture

Ovaj dokument opisuje sve javne funkcije frameworka: listu, stog, red čekanja i pomoćne funkcije.

---

## Zajednički tipovi (`common.h`)

Tri funkcijska pokazivača koja korisnik proslijeđuje pri kreiranju svake strukture:

- **`cmp_fn`** - uspoređuje dva elementa (vraća `<0`, `0` ili `>0`)
- **`print_fn`** - ispisuje jedan element
- **`free_fn`** - oslobađa memoriju jednog elementa (može biti `NULL`)

Sve funkcije vraćaju `Status`:

- `STATUS_OK` - uspjeh
- `STATUS_EMPTY` - struktura je prazna
- `STATUS_FULL` - struktura je puna (samo array implementacije)
- `STATUS_ALLOC_ERROR` - alokacija nije uspjela
- `STATUS_NULL_ARG` - proslijeđen je `NULL` argument

---

## Lista (`list.h`)

Lista omogućuje umetanje i uklanjanje s oba kraja, te pristup po indeksu.

**Dvije implementacije:** `LIST_IMPL_SINGLY` (jednostruka) ili `LIST_IMPL_DOUBLY` (dvostruka).

```c
List* list_create(ListImplType impl, cmp_fn cmp, print_fn print, free_fn destructor);
```

Funkcije:

- `list_destroy(list)` - uništava listu
- `list_insert_front(list, data)` - dodaje na početak
- `list_insert_back(list, data)` - dodaje na kraj
- `list_remove_front(list, &out)` - uklanja s početka
- `list_remove_back(list, &out)` - uklanja s kraja
- `list_get(list, index, &out)` - dohvaća element na indeksu
- `list_is_empty(list)` - je li prazna
- `list_size(list)` - broj elemenata
- `list_print(list)` - ispisuje sve elemente

---

## Stog (`stack.h`)

Stog (LIFO) - dodaje i uklanja elemente samo s vrha.

**Dvije implementacije:** `STACK_IMPL_ARRAY` ili `STACK_IMPL_LINKED_LIST`.

```c
Stack* stack_create(StackImplType impl, cmp_fn cmp, print_fn print, free_fn destructor, size_t capacity);
```

Funkcije:

- `stack_destroy(stack)` - uništava stog
- `stack_push(stack, data)` - dodaje na vrh
- `stack_pop(stack, &out)` - uklanja s vrha
- `stack_peek(stack, &out)` - čita vrh bez uklanjanja
- `stack_is_empty(stack)` - je li prazan
- `stack_size(stack)` - broj elemenata
- `stack_print(stack)` - ispisuje sve elemente

Korisnički kod je identičan za obje implementacije - razlika je samo u pozivu `stack_create()`.

---

## Red čekanja (`queue.h`)

Red (FIFO) - dodaje na kraj, uklanja s početka.

**Dvije implementacije:** `QUEUE_IMPL_ARRAY` ili `QUEUE_IMPL_LIST`.

```c
Queue* queue_create(QueueImplType impl, cmp_fn cmp, print_fn print, free_fn destructor, size_t capacity);
```

Funkcije:

- `queue_destroy(queue)` - uništava red
- `queue_enqueue(queue, data)` - dodaje na kraj
- `queue_dequeue(queue, &out)` - uklanja s početka
- `queue_front(queue, &out)` - čita početak bez uklanjanja
- `queue_is_empty(queue)` - je li prazan
- `queue_size(queue)` - broj elemenata
- `queue_print(queue)` - ispisuje sve elemente

Isto kao kod stoga - korisnički kod ostaje isti, mijenja se samo `impl` argument.

---

## Pomoćne funkcije (`utils.h`)

Gotove `cmp_fn` / `print_fn` / `free_fn` funkcije za int i string:

- `int_cmp` / `string_cmp` - usporedba
- `int_print` / `string_print` - ispis
- `generic_free` - oslobađanje memorije (radi za bilo koji dinamički alociran tip)

### Primjer

```c
#include "stack.h"
#include "utils.h"

Stack* s = stack_create(STACK_IMPL_ARRAY, int_cmp, int_print, NULL, 10);

int x = 5;
stack_push(s, &x);

stack_print(s);
stack_destroy(s);
```

---

## Vlasništvo memorije

- **Umetanje** (`insert`, `push`, `enqueue`) - korisnik ostaje vlasnik podataka.
- **Uklanjanje** (`remove`, `pop`, `dequeue`) - korisnik ponovno postaje vlasnik; `destructor` se ne poziva.
- **Uništavanje** (`destroy`) - sve što je još unutra se oslobađa pozivom `destructor`-a (ako je proslijeđen).