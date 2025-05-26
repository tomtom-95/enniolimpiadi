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
name_save(NameChunkState *names, String str) {
    u64 bytes_left = str.len;
    u64 needed_chunks = (str.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;

    Name name = {.head = NULL, .len = bytes_left, .next = NULL};
    NameChunk **chunk = &(name.head);
    for (u64 i = 0; i < needed_chunks; ++i) {
        *chunk = names->first_free;
        if (*chunk) {
            names->first_free = names->first_free->next;
        }
        else {
            *chunk = (NameChunk *)arena_push(names->arena, sizeof(NameChunk)); 
        }
        u64 bytes_to_copy = getmin((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy((*chunk)->str, str.str + i * NAME_CHUNK_PAYLOAD_SIZE, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        chunk = &((*chunk)->next);
    }
    *chunk = NULL;
    return name;
}

void
name_delete(NameChunkState *names, Name name) {
    NameChunk **chunk = &(name.head);
    while (*chunk) {
        chunk = &((*chunk)->next);
    }
    *chunk = names->first_free;
    names->first_free = name.head;
}

String
name_cat(Arena *arena, Name name) {
    u64 bytes_left = name.len;
    u64 needed_chunks = (bytes_left + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;
    String str = {.str = arena_push(arena, bytes_left), .len = bytes_left};
    NameChunk *chunk = name.head;
    for (u64 i = 0; i < needed_chunks; ++i) {
        u64 bytes_to_copy = getmin((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy(str.str + i * NAME_CHUNK_PAYLOAD_SIZE, chunk->str, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        chunk = chunk->next;
    }
    return str;
}

bool
name_cmp(Name name1, Name name2) {
    if (name1.len != name2.len) {
        return false;
    }

    u64 needed_chunks = (name1.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;
    NameChunk *chunk1 = name1.head;
    NameChunk *chunk2 = name2.head;
    for (u64 i = 0; i < needed_chunks; ++i) {
        if (memcmp(chunk1, chunk2, NAME_CHUNK_PAYLOAD_SIZE)) {
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
namelist_append_right(
    NameState *name_state,
    NameChunkState *name_chunk_state,
    NameList *namelist,
    String string
) {
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
namelist_pop(
    NameState *name_state,
    NameChunkState *name_chunk_state,
    NameList *namelist
) {
    Name **node = &(namelist->head);
    if (*node) {
        while ((*node)->next) {
            node = &((*node)->next);
        }
        Name *tmp = *node;
        *node = NULL;
        name_delete(name_chunk_state, *tmp);
        tmp->next = name_state->first_free;
        name_state->first_free = tmp;
    }
}

#endif // NAMES_V2
