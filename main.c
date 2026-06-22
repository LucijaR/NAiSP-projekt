#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"

// Definiramo strukturu za demonstraciju
typedef struct {
    int id;
    char ime[32];
    char tip_zahtjeva[64];
} StudentDemo;

// Pomoćna funkcija za kreiranje studenta
static StudentDemo* kreiraj_studenta(int id, const char* ime, const char* zahtjev) {
    StudentDemo* s = (StudentDemo*)malloc(sizeof(StudentDemo));
    if (s) {
        s->id = id;
        snprintf(s->ime, sizeof(s->ime), "%s", ime);
        snprintf(s->tip_zahtjeva, sizeof(s->tip_zahtjeva), "%s", zahtjev);
    }
    return s;
}

// Funkcija za ispis studenta prilagođena našim strukturama
static void ispisi_studenta(const void* data) {
    const StudentDemo* s = (const StudentDemo*)data;
    printf("[ID: %d | %s -> Zahtjev: %s]\n", s->id, s->ime, s->tip_zahtjeva);
}

int main(void) {
    printf("==================================================\n");
    printf("  ZAVRŠNI DEMONSTRACIJSKI PROGRAM - REFERADA 2026 \n");
    printf("==================================================\n\n");

    // Listu radimo kao DOUBLY, Queue kao LIST-based, a Stack kao ARRAY-based (kapacitet 10)
    List* baza_studenata = list_create(LIST_IMPL_DOUBLY, NULL, ispisi_studenta, generic_free);
    Queue* red_cekanja = queue_create(QUEUE_IMPL_LIST, NULL, ispisi_studenta, generic_free, 0);
    Stack* kup_dokumenata = stack_create(STACK_IMPL_ARRAY, NULL, ispisi_studenta, generic_free, 10);

    if (!baza_studenata || !red_cekanja || !kup_dokumenata) {
        printf("Greška pri inicijalizaciji struktura!\n");
        return EXIT_FAILURE;
    }

    // 2. KORAK: Unos studenata u glavnu bazu podataka (LISTA)
    printf("--- Korak 1: Punjenje baze podataka studenata (Lista) ---\n");
    list_insert_back(baza_studenata, kreiraj_studenta(101, "Ana Anic", "Potvrda o statusu studenta"));
    list_insert_back(baza_studenata, kreiraj_studenta(102, "Marko Maric", "Upisni list / Promjena smjera"));
    list_insert_back(baza_studenata, kreiraj_studenta(103, "Iva Ivic", "Molba za polaganje pred prorektorom"));
    
    printf("Trenutno stanje u bazi (Lista):\n");
    list_print(baza_studenata);
    printf("\n");

    // 3. KORAK: Studenti dolaze u referadu i staju u red (RED ČEKANJA - FIFO)
    printf("--- Korak 2: Studenti ulaze u red čekanja ispred referade (Red) ---\n");
    for (size_t i = 0; i < list_size(baza_studenata); i++) {
        void* student_iz_baze = NULL;
        list_get(baza_studenata, i, &student_iz_baze);
        
        // Moramo napraviti kopiju za red jer strukture preuzimaju vlasništvo nad memorijom
        StudentDemo* s = (StudentDemo*)student_iz_baze;
        queue_enqueue(red_cekanja, kreiraj_studenta(s->id, s->ime, s->tip_zahtjeva));
    }
    
    printf("Stanje u redu ispred referade (Red):\n");
    queue_print(red_cekanja);
    printf("\n");

    // 4. KORAK: Referentica obrađuje studente jednog po jednog i stavlja papire na stol prodekanice (STOG - LIFO)
    printf("--- Korak 3: Referentica obrađuje red i slaže papire prodekanici na stol (Stog) ---\n");
    while (!queue_is_empty(red_cekanja)) {
        void* obradjeni_student = NULL;
        queue_dequeue(red_cekanja, &obradjeni_student);
        
        StudentDemo* s = (StudentDemo*)obradjeni_student;
        printf("  > Obradjen student %s, zahtjev proslijedjen na potpis.\n", s->ime);
        
        stack_push(kup_dokumenata, obradjeni_student); // Stog preuzima ovaj pokazivač
    }
    printf("\nKup dokumenata na stolu prodekanice (Stog - odozgo prema dolje):\n");
    stack_print(kup_dokumenata);
    printf("\n");

    // 5. KORAK: Prodekanica potpisuje dokumente s vrha kupa
    printf("--- Korak 4: Prodekanica potpisuje dokumente s vrha kupa (LIFO) ---\n");
    while (!stack_is_empty(kup_dokumenata)) {
        void* dokument = NULL;
        stack_pop(kup_dokumenata, &dokument);
        
        StudentDemo* s = (StudentDemo*)dokument;
        printf("  [POTPISANO] %s (ID: %d) - %s\n", s->ime, s->id, s->tip_zahtjeva);
        
        free(s); // Ručno oslobađamo jer smo napravili pop
    }

    printf("\n--- Korak 5: Zatvaranje ureda i uništavanje struktura ---\n");
    list_destroy(baza_studenata);
    queue_destroy(red_cekanja);
    stack_destroy(kup_dokumenata);

    printf("Demonstracijski program je uspješno završio rad bez grešaka!\n");
    return EXIT_SUCCESS;
}