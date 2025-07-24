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
string_from_name(Name name, Arena *arena)
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
namelist_push_front(NameList *namelist, NameNode *node,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    if (namelist->tail == &namelist->sentinel) {
        namelist->tail = node;
    }

    node->next = namelist->sentinel.next;
    namelist->sentinel.next = node;
    ++namelist->len;
}

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

void
namearray_init(Arena *arena, NameArray *namearray, u64 len)
{
    namearray->cnt = 0;
    namearray->len = len;
    namearray->first = arena_push(arena, len * sizeof *namearray->first);
}

void
namearray_insert_from_string(NameArray *namearray, NameState *name_state, 
    NameChunkState *name_chunk_state, String string, u64 pos)
{
    assert(pos < namearray->len);

    Name name = name_from_string(string, name_chunk_state);
    memcpy(namearray->first + pos, &name, sizeof name);
}

void
namearray_insert_from_name(NameArray *namearray, NameState *name_state,
    NameChunkState *name_chunk_state, Name name, u64 pos)
{
    assert(pos < namearray->len);
    name_copy(namearray->first + pos, &name, name_chunk_state);
    ++namearray->cnt;
}


void
insert_player_into_tournament_array(Arena *arena, NameArray *namearray,
    Name player_name, NameChunkState *name_chunk_state)
{
    assert(namearray->len != 0);

    if (namearray->cnt == 0) {
        name_copy(namearray->first, &player_name, name_chunk_state);
    }
    else if (namearray->cnt < namearray->len / 2 + 1) {
        u64 round_0_cnt = namearray->len / 2 + 1;
        u64 round_1_cnt = round_0_cnt / 2;

        // move stuff around in the old array
        u64 players_on_round_1 = round_0_cnt - namearray->cnt;
        u64 players_on_round_0 = namearray->cnt - players_on_round_1;

        u64 dst_position = namearray->len - 1 - players_on_round_0;
        u64 src_position = dst_position / 2 - 1;

        name_copy(namearray->first + dst_position, namearray->first + src_position, name_chunk_state);

        name_release(namearray->first[src_position], name_chunk_state);
        namearray->first[src_position].first_chunk = NULL;
        namearray->first[src_position].last_chunk = NULL;
        namearray->first[src_position].len = 0;

        name_copy(namearray->first + dst_position - 1, &player_name, name_chunk_state);
    }
    else {
        // allocate new array (and for now forget the old one -> memory leak)
        Name *tmp = namearray->first + namearray->len - 1;

        u64 new_len = 2 * namearray->len + 1;
        u64 oldcnt = namearray->cnt;
        namearray_init(arena, namearray, new_len);
        namearray->cnt = oldcnt;

        name_copy(namearray->first + new_len - 1, tmp, name_chunk_state);
        name_copy(namearray->first + new_len - 2, &player_name, name_chunk_state);

        namearray->len = new_len;
    }
    ++namearray->cnt;
}

#endif // NAMES_C
