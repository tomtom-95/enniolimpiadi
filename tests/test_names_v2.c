#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string_v2.c"
#include "../names_v2.c"

int main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena arena = arena_alloc(KiloByte(1));
    Names names = {
        .arena = &arena,
        .first_free = NULL
    };
    String str = string_from_cstring_lit("Helloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo");
    NameChunkList name_chunk_list = name_write(&names, str);
    name_chunk_list.head->next = NULL;
}
