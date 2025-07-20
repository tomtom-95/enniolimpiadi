#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string.c"
#include "../names.c"

void
test_namearray(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    String str1 = string_from_cstring_lit("Hellooooooooooooooooooooooooooooooooooooooooooooooooooooo");
    String str2 = string_from_cstring_lit("Ciao");
    String str3 = string_from_cstring_lit("Ciao Again");

    Name *name1 = name_alloc(str1, &name_state, &name_chunk_state);
    Name *name2 = name_alloc(str2, &name_state, &name_chunk_state);
    Name *name3 = name_alloc(str3, &name_state, &name_chunk_state);

    NameArray name_array = namearray_init(arena, 16);

    namearray_insert(name_array, *name1, 0);
    namearray_insert(name_array, *name2, 1);
    namearray_insert(name_array, *name3, 2);
}

int main(void)
{
    test_namearray();
}