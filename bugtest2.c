#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <stdio.h>

int
main(void) {
    Clay_ElementId new_id = CLAY_ID("test");
    printf("%u\n", new_id.baseId);
    printf("%u\n", new_id.id);
}
