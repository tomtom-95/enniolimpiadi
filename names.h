#ifndef NAMES_H
#define NAMES_H

#include <stddef.h>

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
    NameChunk *first_chunk;
    NameChunk *last_chunk;
    u64 len;
};

typedef struct NameChunkState NameChunkState;
struct NameChunkState {
    Arena *arena; 
    NameChunk *first_free;
};

NameChunkState name_chunk_state_init(Arena *arena);
NameChunk*     name_chunk_alloc(NameChunkState *name_chunk_state);
Name           name_init(String str, NameChunkState *name_chunk_state);
void           name_free(Name name, NameChunkState *name_chunk_state);
void           name_free_scrub(Name *name, NameChunkState *name_chunk_state);
bool           are_name_equal(Name a, Name b);
Name           name_copy(Name src, NameChunkState *name_chunk_state);

String         string_from_name(Arena *arena, Name name);

#endif // NAMES_H
