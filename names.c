#ifndef NAMES_C
#define NAMES_C

#include "names.h"

Name *
name_alloc(String str, NameState *name_state, NameChunkState *name_chunk_state) {
    u64 bytes_left = str.len;
    u64 needed_chunks = (str.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;

    Name *name = name_state->first_free;
    if (name) {
        name_state->first_free = name_state->first_free->next;
    }
    else {
        name = (Name *)arena_push(name_state->arena, sizeof(Name));
    }
    name->len = bytes_left;
    name->next = NULL; // TODO: study carefully this! when resource are acquired be sure to zero initialize!

    NameChunk **chunk = &(name->first_name_chunk);
    for (u64 i = 0; i < needed_chunks; ++i) {
        *chunk = name_chunk_state->first_free;
        if (*chunk) {
            name_chunk_state->first_free = name_chunk_state->first_free->next;
        }
        else {
            *chunk = (NameChunk *)arena_push(name_chunk_state->arena, sizeof(NameChunk)); 
        }
        u64 bytes_to_copy = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy((*chunk)->str, str.str + i * NAME_CHUNK_PAYLOAD_SIZE, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        chunk = &((*chunk)->next);
    }
    *chunk = NULL;
    return name;
}

String
push_string_from_name(Arena *arena, Name name) {
    u64 bytes_left = name.len;
    String str = { .len = bytes_left, .str = arena_push(arena, bytes_left) };
    NameChunk *name_chunk = name.first_name_chunk;
    while (bytes_left) {
        u64 bytes_to_copy = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy(str.str + name.len - bytes_left, name_chunk->str, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        name_chunk = name_chunk->next;
    }
    return str;
}

void
name_delete(Name *name, NameState *name_state, NameChunkState *name_chunk_state)
{
    NameChunk **chunk = &name->first_name_chunk;
    while (*chunk) {
        chunk = &((*chunk)->next);
    }
    *chunk = name_chunk_state->first_free;
    name_chunk_state->first_free = name->first_name_chunk;

    name->next = name_state->first_free;
    name_state->first_free = name;
}

bool
name_cmp(Name *a, Name *b)
{
    if (a->len != b->len) {
        return false;
    }

    NameChunk *ca = a->first_name_chunk;
    NameChunk *cb = b->first_name_chunk;

    u64 bytes_left = a->len;
    while (bytes_left) {
        u64 step = bytes_left < NAME_CHUNK_PAYLOAD_SIZE ? bytes_left : NAME_CHUNK_PAYLOAD_SIZE;
        if (memcmp(ca->str, cb->str, step) != 0) {
            return false;
        }
        bytes_left -= step;
        ca = ca->next;
        cb = cb->next;
    }

    return true;
}

void
namelist_append_right(NameList *name_list, String string,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Name *name = name_alloc(string, name_state, name_chunk_state);
    if (name_list->first_name == NULL) {
        name_list->first_name = name_list->last_name = name;
    }
    else {
        name_list->last_name->next = name;
        name_list->last_name = name_list->last_name->next;
    }
}

void
namelist_append_left(NameList *name_list, String string,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Name *name = name_alloc(string, name_state, name_chunk_state);
    if (name_list->first_name == NULL) {
        name_list->first_name = name_list->last_name = name;
    }
    else {
        name->next = name_list->first_name;
        name_list->first_name = name;
    }
}

Name *
namelist_find(NameList *name_list, String string) {
    Temp temp = scratch_get(0, 0);

    NameState name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState name_chunk_state = {.arena = temp.arena, .first_free = NULL };
    Name *name = name_alloc(string, &name_state, &name_chunk_state);

    Name *node = name_list->first_name;
    while (node) {
        if (name_cmp(name, node)) {
            break;
        }
        node = node->next;
    }

    scratch_release(temp);

    return node;
}

void
namelist_pop_right(NameList *name_list, NameState *name_state, NameChunkState *name_chunk_state)
{
    Name **node = &name_list->first_name;
    while (*node != name_list->last_name) {
        node = &((*node)->next);
    }

    Name *name_to_remove = *node;
    *node = (*node)->next;
    name_list->last_name = (node == &name_list->first_name) ? NULL : ContainerOf(node, Name, next);

    name_delete(name_to_remove, name_state, name_chunk_state);
}

void
namelist_pop_left(NameList *name_list, NameState *name_state, NameChunkState *name_chunk_state)
{
    Name *name_to_remove = name_list->first_name;
    if (name_list->first_name) {
        name_list->first_name = name_list->first_name->next;
        if (!name_list->first_name) {
            name_list->last_name = NULL;
        }
        name_delete(name_to_remove, name_state, name_chunk_state);
    }
}

void
namelist_pop(NameList *name_list, Name *name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Name **node = &name_list->first_name;
    while (*node) {
        if (name_cmp(name, *node)) {
            break;
        }
        node = &((*node)->next);
    }
    *node = (*node)->next;

    if (name == name_list->last_name) {
        name_list->last_name = (node == &name_list->first_name) ? NULL : ContainerOf(node, Name, next);
    }

    name_delete(name, name_state, name_chunk_state);
}

void
namelist_pop_by_string(NameList *name_list, String string,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Temp temp = scratch_get(0, 0);

    NameState temp_name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState temp_name_chunk_state = {.arena = temp.arena, .first_free = NULL };
    Name *name = name_alloc(string, &temp_name_state, &temp_name_chunk_state);

    Name **node = &name_list->first_name;
    while (*node) {
        if (name_cmp(name, *node)) {
            break;
        }
        node = &((*node)->next);
    }

    scratch_release(temp);

    Name *name_to_remove = *node;
    *node = (*node)->next;

    if (name_to_remove == name_list->last_name) {
        name_list->last_name = (node == &name_list->first_name) ? NULL : ContainerOf(node, Name, next);
    }

    name_delete(name_to_remove, name_state, name_chunk_state);
}

void
namelist_delete_all(NameList *name_list, NameState *name_state,
    NameChunkState *name_chunk_state)
{
    while (name_list->first_name) {
        namelist_pop_left(name_list, name_state, name_chunk_state);
    }
}

NameArray
namearray_init(Arena *arena, u64 len)
{
    return (NameArray){ .len = len, .cnt = 0, .first = arena_push(arena, len) };
}

void
namearray_insert(NameArray name_array, Name name)
{

    // from name_array.len I get the current number of players. I am adding 1
    // so I have
    // n: number of players in the tournament
    // from the number of players I can get how big I want the name_array and how many should go
    // into the first round and how many should go into the second round
    Temp scratch = scratch_get(0, 0);

    u64 round_0_cnt = name_array.len / 2 + 1;
    u64 round_1_cnt = round_0_cnt / 2;

    // In which cases do I need to relocate the name_array?
    // If cnt = 0
    //  -> name_array.len = 1
    // -> just put the name in the only available place

    // if cnt = 2
    //  -> name_array.len = 3
    //  -> adding another name means to modify and relocate the array
    //  -> name_array.len must become 3 + (3 + 1)
    // -> what logic do I have to implement to fill the new array?
    if (name_array.cnt > name_array.len / 2) {
        // must realloc bigger array
    }
    else {
        // calculate how many elements must be put in round 0 and how many elements must be put in round 1
        u64 cnt_player_round_1 = round_0_cnt - name_array.cnt;
        u64 cnt_player_round_0 = name_array.cnt - cnt_player_round_1;

        // find the index ranges for players in round 0
        u64 idx_round_0_start = name_array.len - 1 - cnt_player_round_0;
        u64 idx_round_0_end = name_array.len - 1;

        // find the index ranges for players in round 1
        u64 idx_round_1_start = idx_round_0_start - round_1_cnt;
        u64 idx_round_1_end = idx_round_1_start + cnt_player_round_1 - 1;

        StringList string_list = {0};

        for (u64 i = idx_round_0_start; i < idx_round_0_end + 1; ++i) {
            String string = push_string_from_name(scratch.arena, name_array.first[i]);
            string_list_push(scratch.arena, &string_list, string);
        }
        for (u64 i = idx_round_1_start; i < idx_round_1_start + 1; ++i) {
            String string = push_string_from_name(scratch.arena, name_array.first[i]);
            string_list_push(scratch.arena, &string_list, string);
        }
        String string = push_string_from_name(scratch.arena, name);
        string_list_push(scratch.arena, &string_list, string);

        memset(name_array.first, 0, sizeof(Name) * name_array.len);
        ++name_array.cnt;

        // given name_array.cnt and name_array.len I must calculate the idxs again
        u64 idx_round_0_start_v2 = idx_round_0_start - 2;
        u64 idx_round_0_end_v2 = idx_round_0_end;

        u64 idx_round_1_start_v2 = idx_round_1_start;
        u64 idx_round_1_end_v2 = idx_round_1_end - 1;

        // now I must insert all the string in the string_list into the name_array
        StringNode *node = string_list.head;
        for (u64 i = idx_round_0_start_v2; i < idx_round_0_end_v2 + 1; ++i) {
            name_array.first[i] = node
        }
    }
}

#endif // NAMES_C
