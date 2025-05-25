#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string.c"
#include "../names.c"

int main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena arena = arena_alloc(KiloByte(1));
    Names names = {
        .arena = &arena,
        .first_free = NULL
    };
    String str1 = string_from_cstring_lit(
        "HellooooooooooooooooooooooooooooooooooooooooooooooooooooCiaoHelloAgain"
    );
    NameChunkList name1 = name_save(&names, str1);
    String res = name_cat(&arena, name1);
    printf("%s", res.str);
}
