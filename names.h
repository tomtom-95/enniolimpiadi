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
    NameChunk *first_name_chunk;
    u64 len;
    Name *next;
};

typedef struct NameList NameList;
struct NameList {
    Name *first_name;
    Name *last_name;
};

typedef struct NameChunkState NameChunkState;
struct NameChunkState {
    Arena *arena; 
    NameChunk *first_free;
};

typedef struct NameState NameState;
struct NameState {
    Arena *arena;
    Name *first_free;
};

Name *name_alloc(String str, NameState *name_state, NameChunkState *name_chunk_state);
String push_string_from_name(Arena *arena, Name name);

void name_delete(Name *name, NameState *name_state, NameChunkState *name_chunk_state);
bool name_cmp(Name *a, Name *b);

void namelist_append_right(NameList *name_list, String string, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_append_left(NameList *name_list, String string, NameState *name_state, NameChunkState *name_chunk_state);

Name *namelist_find(NameList *name_list, String string);

void namelist_pop(NameList *name_list, Name *name, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_pop_by_string(NameList *name_list, String string, NameState *name_state, NameChunkState *name_chunk_state);

void namelist_pop_left(NameList *name_list, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_pop_right(NameList *name_list, NameState *name_state, NameChunkState *name_chunk_state);

void namelist_delete_all(NameList *name_list, NameState *name_state, NameChunkState *name_chunk_state);

#endif // NAMES_H
