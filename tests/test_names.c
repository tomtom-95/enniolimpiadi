#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string.c"
#include "../names.c"

int main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena arena = arena_alloc(KiloByte(1));
    NameChunkState name_chunk_state = {.arena = &arena, .first_free = NULL};
    NameState name_state = {.arena = &arena, .first_free = NULL};

    String str1 = string_from_cstring_lit("Hello");
    String str2 = string_from_cstring_lit("Ciao");

    NameList namelist = {0};
    namelist_append_right(&name_state, &name_chunk_state, &namelist, str1);
    namelist_append_right(&name_state, &name_chunk_state, &namelist, str2);
    namelist_pop(&name_state, &name_chunk_state, &namelist);
}
