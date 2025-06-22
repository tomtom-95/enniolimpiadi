#ifndef NAMES_V2
#define NAMES_V2

#include "utils.c"
#include "arena.c"
#include "string.c"

#define NAME_CHUNK_PAYLOAD_SIZE 56

typedef struct NameChunk NameChunk;
struct NameChunk {
    u8 str[NAME_CHUNK_PAYLOAD_SIZE];
    NameChunk *next;
};

typedef struct Name Name;
struct Name {
    NameChunk *head;
    u64 len;
    Name *next;
};

typedef struct NameChunkState NameChunkState;
struct NameChunkState {
    Arena *arena; 
    NameChunk *first_free;
};

typedef struct NameList NameList;
struct NameList {
    Name *head;
};

typedef struct NameState NameState;
struct NameState {
    Arena *arena;
    Name *first_free;
};

Name
name_save(NameChunkState *name_chunk_state, String str) {
    u64 bytes_left = str.len;
    u64 needed_chunks = (str.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;

    Name name = {.head = NULL, .len = bytes_left, .next = NULL};
    NameChunk **chunk = &(name.head);
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
name_to_string(Arena *arena, Name name) {
    u64 bytes_left = name.len;
    String str = {.len = bytes_left, .str = arena_push(arena, bytes_left)};
    NameChunk *name_chunk = name.head;
    while (bytes_left) {
        u64 bytes_to_copy = Min((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy(str.str + name.len - bytes_left, name_chunk->str, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        name_chunk = name_chunk->next;
    }
    return str;
}

void
name_delete(NameChunkState *name_chunk_state, Name *name) {
    NameChunk **chunk = &(name->head);
    while (*chunk) {
        chunk = &((*chunk)->next);
    }
    *chunk = name_chunk_state->first_free;
    name_chunk_state->first_free = name->head;
    name->head = NULL; // not necessary: but useful?
}

bool
name_cmp(Name name1, Name name2) {
    if (name1.len != name2.len) {
        return false;
    }

    u64 bytes_left = name1.len;
    u64 needed_chunks = (name1.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;
    NameChunk *chunk1 = name1.head;
    NameChunk *chunk2 = name2.head;
    for (u64 i = 0; i < needed_chunks; ++i) {
        if (memcmp(chunk1, chunk2, Min(bytes_left, (u64)NAME_CHUNK_PAYLOAD_SIZE))) {
            return false;
        }
        else {
            chunk1 = chunk1->next;
            chunk2 = chunk2->next;
        }
    }
    return true;
}

void
namelist_append_right(NameState *name_state, NameChunkState *name_chunk_state, NameList *namelist, String string) {
    Name name = name_save(name_chunk_state, string);

    Name **node = &(namelist->head);
    while (*node) {
        node = &((*node)->next);
    }
    *node = name_state->first_free;
    if (*node) {
        name_state->first_free = name_state->first_free->next;
    }
    else {
        *node = arena_push(name_state->arena, sizeof(Name));
    }
    (*node)->head = name.head;
    (*node)->len = name.len;
    (*node)->next = NULL;
}

void
namelist_pop_right(NameState *name_state, NameChunkState *name_chunk_state, NameList *namelist) {
    Name **node = &(namelist->head);
    if (*node) {
        while ((*node)->next) {
            node = &((*node)->next);
        }
        Name *to_remove = *node;
        *node = NULL;
        name_delete(name_chunk_state, to_remove);
        to_remove->next = name_state->first_free;
        name_state->first_free = to_remove;
    }
}

void
namelist_pop(NameState *name_state, NameChunkState *name_chunk_state, NameList *namelist, String str) {
    Name name = name_save(name_chunk_state, str);
    Name **node = &(namelist->head);
    while (*node) {
        if (name_cmp(**node, name)) {
            Name *to_remove = *node;
            *node = (*node)->next;
            name_delete(name_chunk_state, to_remove);
            to_remove->next = name_state->first_free;
            name_state->first_free = to_remove;
        }
        else {
            node = &((*node)->next);
        }
    }
    name_delete(name_chunk_state, &name);
}

void
namelist_delete(NameState *name_state, NameChunkState *name_chunk_state, NameList *namelist) {
    Name *node = namelist->head;
    while (node) {
        Name *to_remove = node;
        node = node->next;
        name_delete(name_chunk_state, to_remove);
        to_remove->next = name_state->first_free;
        name_state->first_free = to_remove;
    }
    namelist->head = NULL; // not necessary, but useful?
}

#endif // NAMES_V2
