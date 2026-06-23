# Arhitektura Frameworka

Kratak opis kako je framework dizajniran i zašto.

---

## Cilj

Korisnik koristi stog, listu ili red bez da zna kako su iznutra napravljeni. Na primjer, stog može biti napravljen pomoću niza ili pomoću povezane liste - kod koji ga koristi (`stack_push`, `stack_pop`...) ostaje isti u oba slučaja.

---

## Struktura projekta

```
include/
  common.h
  list.h
  queue.h
  stack.h
  utils.h
  internal/
    list_internal.h
    list_singly_internal.h
    list_doubly_internal.h
    queue_internal.h
    queue_array_internal.h
    queue_list_internal.h
    stack_internal.h
    stack_array_internal.h
    stack_list_internal.h

src/
  list.c
  list_singly.c
  list_doubly.c
  queue.c
  queue_array.c
  queue_list.c
  stack.c
  stack_array.c
  stack_list.c
  utils.c

tests/
  main_test.c
  int_test.c
  string_test.c
  custom_type_test.c
```

**include/** - javni i privatni headeri.
**src/** - implementacija.
**tests/** - test programi.

Svaka struktura (lista, stog, red) ima tri dijela:

1. Javni header (npr. `stack.h`) - ono što korisnik koristi
2. Privatni header (npr. `stack_internal.h`) - stvarna struktura, skrivena od korisnika
3. Konkretne implementacije (npr. `stack_array.c`, `stack_list.c`) - stvarni kod

---

## Generičnost

Sve strukture spremaju `void*` umjesto konkretnog tipa. Tako ista lista može spremati brojeve, stringove ili vlastite strukture.

Međutim, framework ne zna kako usporediti, ispisati ili osloboditi te podatke. Zato korisnik mora proslijediti tri funkcije.

---

## Funkcijski pokazivači

Kod kreiranja strukture, korisnik proslijedi:

- funkciju za usporedbu dva elementa
- funkciju za ispis elementa
- funkciju za oslobađanje memorije elementa

Framework ih poziva u pravom trenutku, bez da sam zna tip podatka.

`utils.h` već ima gotove verzije ovih funkcija za int i string.

---

## Vtable - zamjena implementacije

Ovo je glavni obrazac u frameworku.

Stog ima dvije implementacije (array i lista). Obje rade `push`, `pop`, `peek`, ali svaka na svoj način. Problem koju verziju `stack_push()` treba pozvati rješava se tablicom pokazivača na funkcije.

```c
typedef struct {
    Status (*push)(void* impl, const void* data);
    Status (*pop)(void* impl, void** out);
    Status (*peek)(void* impl, void** out);
    // ...
} StackVtable;
```

Svaka implementacija ima svoju popunjenu tablicu:

```c
const StackVtable stack_array_vtable = {
    .push = stack_array_push,
    .pop  = stack_array_pop,
};

const StackVtable stack_list_vtable = {
    .push = stack_list_push,
    .pop  = stack_list_pop,
};
```

Generički `Stack` struct pamti koju tablicu koristi:

```c
struct Stack {
    void* impl;
    const StackVtable* vt;
};
```

Kod kreiranja, `stack_create()` postavi pravu tablicu:

```c
switch (impl) {
    case STACK_IMPL_ARRAY:
        stack->vt = &stack_array_vtable;
        break;
    case STACK_IMPL_LINKED_LIST:
        stack->vt = &stack_list_vtable;
        break;
}
```

Svaka javna funkcija samo prosljeđuje poziv dalje:

```c
Status stack_push(Stack* stack, const void* data)
{
    return stack->vt->push(stack->impl, data);
}
```

Korisnik koji zove `stack_push()` ne zna i ne treba znati je li ispod toga array ili lista.

Isti princip vrijedi i za red čekanja (`queue.c`).

---

## Enkapsulacija

Korisnik nikad ne vidi unutrašnjost strukture. Javni header ima samo:

```c
typedef struct Stack Stack;
```

Ovo je opaque tip - postoji, ali korisnik ne vidi njegova polja (ta su definirana samo u `internal/stack_internal.h`).

Korisnik može samo:
- dobiti `Stack*` preko `stack_create()`
- prosljeđivati ga funkcijama
- ne može direktno čitati ili mijenjati polja

---

## Sažetak

1. Korisnik zove `stack_create(STACK_IMPL_ARRAY, ...)`
2. `stack_create()` postavi `vt` na `stack_array_vtable`
3. Korisnik zove `stack_push(s, &x)`
4. To se prosljeđuje kroz `s->vt->push(...)`
5. Stvarno se pozove `stack_array_push()`

Da je korisnik odabrao `STACK_IMPL_LINKED_LIST`, koraci 3 i 4 bi bili isti, samo bi se na kraju pozvala `stack_list_push()` umjesto.