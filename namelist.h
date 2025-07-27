#ifndef NAMELIST_H
#define NAMELIST_H

#include "names.h"

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

typedef struct NameState NameState;
struct NameState {
    Arena *arena;
    NameNode *first_free_name_node;
    NameChunkState name_chunk_state;
};

NameState name_state_init(Arena *arena);
void      namelist_init(NameList *namelist);
NameNode* name_node_alloc(NameState *name_state);
void      name_node_free(NameNode *node, NameState *name_state);
NameNode* namelist_find(NameList *namelist, Name name);
void      namelist_push_front(NameList *namelist, Name name, NameState *name_state);
void      namelist_pop_front(NameList *namelist, NameState *name_state);
void      namelist_pop(NameList *namelist, Name name, NameState *name_state);
void      namelist_delete(NameList *namelist, NameState *name_state);

#endif // NAMELIST_H
