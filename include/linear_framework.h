#ifndef LINEAR_FRAMEWORK_H
#define LINEAR_FRAMEWORK_H

#include <stdbool.h>
#include <stddef.h>

typedef struct LinearStructure* LinearStruct;

LinearStruct create_test_stack(void);
void pozdravi_iz_frameworka(LinearStruct ls);
void free_test_stack(LinearStruct ls);

#endif