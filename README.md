# Linear Framework za liste, redove i stogove

Ovaj projekt je generički C framework za rad s-linearnim strukturama podataka: listom, redom čekanja i stogom. Omogućuje rad s različitim implementacijama istog API-ja, bez potrebe da korisnik zna kako su strukture interno organizirane.

## Što ovaj projekt nudi

- generičke strukture podataka koje rade s `void*`
- podršku za više implementacija iste strukture
- jednostavan i konzistentan API za listu, red i stog
- testove za cijeli projekt
- demonstracijski program koji pokazuje korištenje frameworka u praksi

## Podržane strukture

### Lista
- jednostruko povezanu listu (`LIST_IMPL_SINGLY`)
- dvostruko povezanu listu (`LIST_IMPL_DOUBLY`)

### Red čekanja
- array-based implementaciju (`QUEUE_IMPL_ARRAY`)
- list-based implementaciju (`QUEUE_IMPL_LIST`)

### Stog
- array-based implementaciju (`STACK_IMPL_ARRAY`)
- linked-list-based implementaciju (`STACK_IMPL_LINKED_LIST`)

## Struktura projekta

```text
include/        - javni i privatni headeri
src/            - implementacija frameworka
tests/          - test programi
main.c          - demonstracijski program
CMakeLists.txt  - build konfiguracija
```

## Preduvjeti

Za izgradnju projekta potrebno je imati instaliran:

- CMake
- C compiler (npr. GCC ili Clang)

## Izgradnja projekta

U korijenskom direktoriju projekta pokreni:

```bash
cmake -S . -B build
cmake --build build
```

## Pokretanje testova

```bash
ctest --test-dir build --output-on-failure
```

## Pokretanje demo programa

```bash
./build/demo_program
```

## Primjer korištenja

```c
#include "stack.h"
#include "utils.h"

Stack* s = stack_create(STACK_IMPL_ARRAY, int_cmp, int_print, NULL, 10);
int x = 5;

stack_push(s, &x);
stack_print(s);
stack_destroy(s);
```

## Napomene

- Framework koristi funkcijske pokazivače za usporedbu, ispis i oslobađanje elemenata.
- Za osnovne tipove kao što su `int` i `char*` postoje već pripremljene pomoćne funkcije u [include/utils.h](include/utils.h).
- U projektu se koristi opaque API, što znači da korisnik ne pristupa izravno unutarnjoj strukturi podataka.

## Autor

Projekt je namijenjen kao studentski primjer generičkog programiranja u C-u s linearnim strukturama podataka.
