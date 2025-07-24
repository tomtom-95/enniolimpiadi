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

typedef struct NameNode NameNode;
struct NameNode {
    Name name;
    NameNode *next;
};

typedef struct NameList NameList;
struct NameList {
    NameNode sentinel;
    NameNode *tail;
    u64 len;
};

typedef struct NameArray NameArray;
struct NameArray {
    Name *first;
    u64 len;
    u64 cnt;
};

typedef struct NameChunkState NameChunkState;
struct NameChunkState {
    Arena *arena; 
    NameChunk *first_free;
};

typedef struct NameState NameState;
struct NameState {
    Arena *arena;
    NameNode *first_free;
};


void name_chunk_state_init(Arena *arena, NameChunkState *state);
void name_state_init(Arena *arena, NameState *state);

NameChunk *name_chunk_alloc(NameChunkState *state);

Name name_from_string(String str, NameChunkState *state);
void name_release(Name name, NameChunkState *state);
String string_from_name(Name name, Arena *arena);
bool are_name_equal(Name *a, Name *b);

NameNode *name_node_alloc(NameState *name_state);
void name_node_init(NameNode *node, String str, NameChunkState *name_chunk_state);
void name_node_release(NameNode *node, NameState *name_state, NameChunkState *name_chunk_state);

void namelist_init(NameList *namelist);
NameNode *namelist_find(NameList *namelist, Name name);
void namelist_push_front(NameList *namelist, NameNode *node, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_pop_front(NameList *namelist, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_pop(NameList *namelist, Name *name, NameState *name_state, NameChunkState *name_chunk_state);
void namelist_delete(NameList *namelist, NameState *name_state, NameChunkState *name_chunk_state);

#endif // NAMES_H
