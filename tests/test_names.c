#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string.c"
#include "../names.c"

#if 0

void
test_namelist_append_right(void) {
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    String str1 = string_from_cstring_lit("Hellooooooooooooooooooooooooooooooooooooooooooooooooooooo");
    String str2 = string_from_cstring_lit("Ciao");
    String str3 = string_from_cstring_lit("Ciao Again");

    NameList name_list;
    namelist_append_right(&name_list, str1, &name_state, &name_chunk_state);
    namelist_append_right(&name_list, str2, &name_state, &name_chunk_state);
    namelist_append_right(&name_list, str3, &name_state, &name_chunk_state);
}

// void
// test_namelist_pop_last_element(void) {
//     Arena *arena = arena_alloc(KiloByte(1));
// 
//     Ctx ctx = {0};
//     ctx_init(&ctx);
// 
//     NameState name_state = {.arena = arena, .first_free = NULL};
//     NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};
// 
//     String str1 = string_from_cstring_lit("Ciao1");
//     String str2 = string_from_cstring_lit("Ciao2");
// 
//     NameList name_list = {0};
//     namelist_append_right(&name_list, str1, &name_state, &name_chunk_state);
//     namelist_append_right(&name_list, str2, &name_state, &name_chunk_state);
// 
//     Name *name = namelist_find(&name_list, str2);
//     namelist_pop(&name_list, name, &name_state, &name_chunk_state);
// }

void
test_namelist_single_element_pop(void) {
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list = {0};

    String str1 = string_from_cstring_lit("Ciao1");
    namelist_append_right(&name_list, str1, &name_state, &name_chunk_state);

    Name *name_to_delete = namelist_find(&name_list, str1);
    namelist_pop(&name_list, name_to_delete, &name_state, &name_chunk_state);
}

void
test_namelist_pop(void) {
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list_0 = {0};

    String str1 = string_from_cstring_lit("Ciao1");
    String str2 = string_from_cstring_lit("Ciao2");

    // Test deletion of last element in two element list
    namelist_append_right(&name_list_0, str1, &name_state, &name_chunk_state);
    namelist_append_right(&name_list_0, str2, &name_state, &name_chunk_state);

    Name *name_to_delete_0 = namelist_find(&name_list_0, str2);
    namelist_pop(&name_list_0, name_to_delete_0, &name_state, &name_chunk_state);

    // Test deletion of element in one element list
    NameList name_list_1 = {0};
    namelist_append_right(&name_list_1, str1, &name_state, &name_chunk_state);
    Name *name_to_delete_1 = namelist_find(&name_list_1, str1);
    namelist_pop(&name_list_1, name_to_delete_1, &name_state, &name_chunk_state);
}

void
test_namelist_pop_using_string(void) {
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list_0 = {0};

    String str1 = string_from_cstring_lit("Ciao1");
    String str2 = string_from_cstring_lit("Ciao2");

    // Test deletion of last element in two element list
    namelist_append_right(&name_list_0, str1, &name_state, &name_chunk_state);
    namelist_append_right(&name_list_0, str2, &name_state, &name_chunk_state);

    namelist_pop_by_string(&name_list_0, str2, &name_state, &name_chunk_state);
}

void
test_namelist_find_non_existent_element(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list = {0};

    String str0 = string_from_cstring_lit("Wrong");
    String str1 = string_from_cstring_lit("Ciao");
    namelist_append_right(&name_list, str1, &name_state, &name_chunk_state);

    Name *name_to_delete = namelist_find(&name_list, str0);
}

void
test_namelist_delete_all(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list = {0};

    String str0 = string_from_cstring_lit("Wrong");
    String str1 = string_from_cstring_lit("Ciao");
    namelist_append_right(&name_list, str0, &name_state, &name_chunk_state);
    namelist_append_right(&name_list, str1, &name_state, &name_chunk_state);

    namelist_delete_all(&name_list, &name_state, &name_chunk_state);
}

void
test_namelist_pop_left(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};

    NameList name_list = {0};

    namelist_pop_left(&name_list, &name_state, &name_chunk_state);
}

#endif

void
test_names(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameChunkState name_chunk_state;
    name_chunk_state_init(arena, &name_chunk_state);

    NameState name_state;
    name_state_init(arena, &name_state);

    NameList namelist;
    namelist_init(&namelist);

    String str0 = string_from_cstring_lit("Ciao");
    String str1 = string_from_cstring_lit("Hello");

    NameNode *node0 = name_node_alloc(&name_state);
    name_node_init(node0, str0, &name_state);

    NameNode *node1 = name_node_alloc(&name_state);
    name_node_init(node1, str1, &name_state);

    Name samestr1name = name_from_string(str1, &name_chunk_state);

    namelist_push_front(&namelist, node0, &name_state, &name_chunk_state);
    namelist_push_front(&namelist, node1, &name_state, &name_chunk_state);

    namelist_pop_front(&namelist, &name_state, &name_chunk_state);
    namelist_pop_front(&namelist, &name_state, &name_chunk_state);
    namelist_pop_front(&namelist, &name_state, &name_chunk_state);

    NameNode *found = namelist_find(&namelist, samestr1name);
}

#if 0
void
test_namearray(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameChunkState name_chunk_state;
    name_chunk_state_init(arena, &name_chunk_state);

    NameState name_state;
    name_state_init(arena, &name_state);

    NameArray namearray;
    namearray_init(arena, &namearray, 16);

    String str0 = string_from_cstring_lit("Ciao");

    namearray_insert_from_string(&namearray, &name_state, &name_chunk_state, str0, 0);
}
#endif

void
test_name_copy(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameChunkState name_chunk_state;
    name_chunk_state_init(arena, &name_chunk_state);

    String srcstr = string_from_lit_comp(
        "Ciaooooooooooooooooooooooooooooooooooooooooooooooooiiiiiiiiiiiiiiiii"
    );
    Name srcname = name_from_string(srcstr, &name_chunk_state);
    Name dstname;

    name_copy(&dstname, &srcname, &name_chunk_state);
}

void
test_namearray(void)
{
    Arena *arena = arena_alloc(MegaByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameChunkState name_chunk_state;
    name_chunk_state_init(arena, &name_chunk_state);

    NameState name_state;
    name_state_init(arena, &name_state);

    NameArrayState name_array_state;
    name_array_state_init(arena, &name_array_state);

    Name riccardo = name_from_string(string_from_lit_comp("riccado"), &name_chunk_state);
    Name giulio   = name_from_string(string_from_lit_comp("giulio"), &name_chunk_state);
    Name tommaso  = name_from_string(string_from_lit_comp("tommaso"), &name_chunk_state);
    Name davide   = name_from_string(string_from_lit_comp("davide"), &name_chunk_state);

    NameArray *namearray = namearray_init(arena, 1, &name_array_state);

    insert_player_into_tournament_array(arena, namearray, riccardo, &name_chunk_state);
    insert_player_into_tournament_array(arena, namearray, giulio, &name_chunk_state);
    insert_player_into_tournament_array(arena, namearray, tommaso, &name_chunk_state);
    insert_player_into_tournament_array(arena, namearray, davide, &name_chunk_state);
}

int main(void)
{
    test_namearray();
}

