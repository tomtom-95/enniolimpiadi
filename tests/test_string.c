#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../string.c"

int main(void) {
    Arena arena = arena_alloc(KiloByte(1));
    StringList string_list = {0};

    String str1 = string_from_cstring_lit(
        "Hellooooooooooooooooooooooooooooooooooooooooooooooooooooo"
    );
    String str2 = string_from_cstring_lit("Ciao");

    string_list_push(&arena, &string_list, str1);
    string_list_push(&arena, &string_list, str2);
}
