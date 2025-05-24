#include <stdlib.h>
#include <string.h>

#include "../utils.c"
#include "../log.c"
#include "../arena.c"
#include "../string_v2.c"

int
main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena arena = arena_alloc(KiloByte(1));

    String string1 = string_from_cstring_lit("Ciao");
    String string2 = string_from_cstring((u8 *)"Mamma");

    log_info("%s", string1.str);
    log_info("%s", string2.str);

    String arena_string1 = string_cpy(&arena, string1);
    String arena_string2 = string_cpy(&arena, string2);
    String arena_cat = string_cat(&arena, arena_string1, arena_string2);

    log_info("%s", arena_cat.str);
    log_info("%d", string_cmp(arena_string1, arena_string2));
}
