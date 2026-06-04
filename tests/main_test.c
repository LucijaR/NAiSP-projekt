#include <stdio.h>
#include "linear_framework.h"

int main() {
    printf("Pokrećem test program...\n");
    
    LinearStruct moj_stog = create_test_stack();
    
    pozdravi_iz_frameworka(moj_stog);
    
    free_test_stack(moj_stog);
    
    printf("Test uspješno završen!\n");
    return 0;
}