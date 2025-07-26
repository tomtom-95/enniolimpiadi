#ifndef NAMES_C
#define NAMES_C

#include "names.h"

void
name_chunk_state_init(Arena *arena, NameChunkState *state)
{
    state->arena = arena;
    state->first_free = NULL;
}

void
name_state_init(Arena *arena, NameState *state)
{
    state->arena = arena;
    state->first_free = NULL;
}

void
name_array_state_init(Arena *arena, NameArrayState *name_array_state)
{
    name_array_state->arena = arena;
    name_array_state->first_free = NULL;
}

NameChunk *
name_chunk_alloc(NameChunkState *state)
{
    NameChunk *chunk = state->first_free;

    if (chunk) {
        state->first_free = state->first_free->next;
    }
    else {
        chunk = (NameChunk *)arena_push(state->arena, sizeof(NameChunk));
    }

    return chunk;
}

Name
name_from_string(String str, NameChunkState *state)
{
    Name result = { .len = str.len };
    NameChunk **link = &result.first_chunk;

    for (u64 off = 0; off < str.len; off += NAME_CHUNK_PAYLOAD_SIZE) {
        NameChunk *chunk = name_chunk_alloc(state);

        u64 bytes = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, str.len - off);
        memcpy(chunk->str, str.str + off, bytes);

        *link = chunk;
        link  = &chunk->next;
    }

    result.last_chunk = ContainerOf(link, NameChunk, next);
    *link = NULL;

    return result;
}

void
name_release(Name name, NameChunkState *state)
{
    name.last_chunk->next = state->first_free;
    state->first_free = name.first_chunk;
}

String
string_from_name(Arena *arena, Name name)
{
    u64 bytes_left = name.len;
    String str = { .len = bytes_left, .str = arena_push(arena, bytes_left) };
    NameChunk *name_chunk = name.first_chunk;
    while (bytes_left) {
        u64 bytes_to_copy = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy(str.str + name.len - bytes_left, name_chunk->str, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        name_chunk = name_chunk->next;
    }
    return str;
}

bool
are_name_equal(Name *a, Name *b)
{
    if (a->len != b->len) {
        return false;
    }

    NameChunk *ca = a->first_chunk;
    NameChunk *cb = b->first_chunk;

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
name_copy(Name *dst, Name *src, NameChunkState *name_chunk_state)
{
    dst->len = src->len;

    NameChunk **link = &dst->first_chunk;

    NameChunk *src_chunk = src->first_chunk;
    for (u64 off = 0; off < src->len; off += NAME_CHUNK_PAYLOAD_SIZE) {
        NameChunk *chunk = name_chunk_alloc(name_chunk_state);

        u64 bytes = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, src->len - off);
        memcpy(chunk->str, src_chunk->str, bytes);
        src_chunk = src_chunk->next;

        *link = chunk;
        link  = &chunk->next;
    }

    dst->last_chunk = ContainerOf(link, NameChunk, next);
    *link = NULL;
}

void
namelist_init(NameList *namelist)
{
    namelist->sentinel.next = NULL;
    namelist->tail = &(namelist->sentinel);
    namelist->len = 0;
}

NameNode *
name_node_alloc(NameState *name_state)
{
    NameNode *name_node = name_state->first_free;

    if (name_node) {
        name_state->first_free->next = name_node->next;
    }
    else {
        name_node = (NameNode *)arena_push(name_state->arena, sizeof *name_node);
    }

    return name_node;
}

void
name_node_init(NameNode *node, String str,
    NameChunkState *name_chunk_state)
{
    node->name = name_from_string(str, name_chunk_state);
    node->next = NULL;
}

void
name_node_release(NameNode *node, NameState *name_state,
    NameChunkState *name_chunk_state)
{
    name_release(node->name, name_chunk_state);
    node->next = name_state->first_free;
    name_state->first_free = node;
}

NameNode *
namelist_find(NameList *namelist, Name name)
{
    NameNode *node = namelist->sentinel.next;
    while (node) {
        if (are_name_equal(&node->name, &name)) {
            break;
        }
        else {
            node = node->next;
        }
    }

    return node;
}

void
namelist_push_front(NameList *namelist, Name name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    // sloppy
    Temp temp = scratch_get(0, 0);

    String str = string_from_name(temp.arena, name);
    NameNode *name_node = name_node_alloc(name_state);
    name_node_init(name_node, str, name_chunk_state);

    scratch_release(temp);

    if (namelist->tail == &namelist->sentinel) {
        namelist->tail = name_node;
    }

    name_node->next = namelist->sentinel.next;
    namelist->sentinel.next = name_node;
    ++namelist->len;
}

// void
// namelist_push_front(NameList *namelist, NameNode *node,
//     NameState *name_state, NameChunkState *name_chunk_state)
// {
//     if (namelist->tail == &namelist->sentinel) {
//         namelist->tail = node;
//     }
// 
//     node->next = namelist->sentinel.next;
//     namelist->sentinel.next = node;
//     ++namelist->len;
// }

void
namelist_pop_front(NameList *namelist,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    if (namelist->sentinel.next == NULL) {
        return;
    }

    if (namelist->tail == namelist->sentinel.next) {
        namelist->tail = &namelist->sentinel;
    }

    NameNode *node = namelist->sentinel.next;

    --namelist->len;
    namelist->sentinel.next = node->next;
    name_node_release(node, name_state, name_chunk_state);
}

void
namelist_pop(NameList *namelist, Name *name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    NameNode *node = &namelist->sentinel;
    while (node->next) {
        if (are_name_equal(&(node->next->name), name)) {
            break;
        }
        else {
            node = node->next;
        }
    }

    if (node->next) {
        if (namelist->tail == node->next) {
            namelist->tail = node;
        }
        name_node_release(node->next, name_state, name_chunk_state);
        node->next = node->next->next;
    }
}

void
namelist_delete(NameList *namelist, NameState *name_state,
    NameChunkState *name_chunk_state)
{
    while (namelist->sentinel.next) {
        namelist_pop_front(namelist, name_state, name_chunk_state);
    }
}

// NameArray *
// namearray_init(u64 len, NameArrayState *name_array_state)
// {
//     NameArray *namearray = name_array_state->first_free;
//     if (namearray) {
//         name_array_state->first_free = name_array_state->first_free->next;
//     }
//     else {
//         namearray = arena_push(name_array_state->arena,
//             sizeof *namearray + NAME_ARRAY_MAX_SIZE * sizeof(Name));
//     }
// 
//     namearray->next = NULL;
//     namearray->len = len;
//     namearray->cnt = 0;
// 
//     return namearray;
// }

// void
// namearray_release(NameArray *namearray, NameArrayState *name_array_state,
//     NameChunkState *name_chunk_state)
// {
//     namearray->next = name_array_state->first_free;
//     name_array_state->first_free = namearray;
// 
//     Name *first_name = namearray + 1;
// 
//     // Should I walk the namearray and release every name?
//     for (u64 i = 0; i < NAME_ARRAY_MAX_SIZE; ++i) {
//         name_release(*(first_name + i), name_chunk_state);
// 
//         // make sure to never dereference a garbage name chunk.
//         // Is this useful to prevent bugs?
//         memset(first_name + i, 0, sizeof *first_name);
//     }
// }

// void
// namearray_insert_from_string(NameArray *namearray, NameState *name_state, 
//     NameChunkState *name_chunk_state, String string, u64 pos)
// {
//     assert(pos < NAME_ARRAY_MAX_SIZE);
// 
//     if (pos >= namearray->len) {
// 
//     }
//     assert(pos < namearray->len);
// 
//     Name name = name_from_string(string, name_chunk_state);
//     memcpy(namearray->first + pos, &name, sizeof name);
// }
// 
// void
// namearray_insert_from_name(NameArray *namearray, NameState *name_state,
//     NameChunkState *name_chunk_state, Name name, u64 pos)
// {
//     assert(pos < namearray->len);
//     name_copy(namearray->first + pos, &name, name_chunk_state);
//     ++namearray->cnt;
// }


// void
// insert_player_into_player_array(Arena *arena, NameArray *playerarray,
//     Name player_name, NameChunkState *name_chunk_state)
// {
//     assert(playerarray->len != 0 && playerarray->cnt < NAME_ARRAY_MAX_SIZE / 2);
// 
//     if (playerarray->cnt == 0) {
//         name_copy((u8 *)playerarray + sizeof *playerarray, &player_name, name_chunk_state);
//     }
//     else if (playerarray->cnt < playerarray->len / 2 + 1) {
//         u64 round_0_cnt = playerarray->len / 2 + 1;
//         u64 round_1_cnt = round_0_cnt / 2;
// 
//         u64 players_on_round_1 = round_0_cnt - playerarray->cnt;
//         u64 players_on_round_0 = playerarray->cnt - players_on_round_1;
// 
//         u64 dst_position = playerarray->len - 1 - players_on_round_0;
//         u64 src_position = dst_position / 2 - 1;
// 
//         Name *first_name = playerarray + 1;
// 
//         Name *src_p = first_name + src_position;
//         Name *dst_p = first_name + dst_position;
// 
//         name_copy(dst_p, src_p, name_chunk_state);
// 
//         name_release(*src_p, name_chunk_state);
//         memset(src_p, 0, sizeof(Name));
// 
//         name_copy(dst_p - 1, &player_name, name_chunk_state);
//     }
//     else {
//         u64 src_position = playerarray->len - 1;
//         u64 dst_position = 2 * playerarray->len;
// 
//         Name *first_name = playerarray + 1;
// 
//         Name *src_p = first_name + src_position;
//         Name *dst_p = first_name + dst_position;
// 
//         name_copy(dst_p, src_p, name_chunk_state);
// 
//         name_release(*src_p, name_chunk_state);
//         memset(src_p, 0, sizeof(Name));
// 
//         name_copy(dst_p - 1, &player_name, name_chunk_state);
// 
//         playerarray->len = 2 * playerarray->len + 1;
//     }
//     ++playerarray->cnt;
// }


#endif // NAMES_C
