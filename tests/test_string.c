#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../string.c"

int main(void) {
    Arena arena = arena_alloc(KiloByte(1));
    StringList string_list = {0};

    String str = string_from_cstring_lit("  \t\n  Hello  \n  ");
    string_strip(&str);
    return 0;
}
