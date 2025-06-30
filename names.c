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

#endif // NAMES_C
