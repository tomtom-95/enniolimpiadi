#ifndef NAMELIST_C
#define NAMELIST_C

#include "namelist.h"

NameState
name_state_init(Arena *arena)
{
    return (NameState){
        .arena = arena,
        .first_free_name_node  = NULL,
        .name_chunk_state = name_chunk_state_init(arena)
    };
}

void
namelist_init(NameList *namelist)
{
    namelist->sentinel.next = NULL;
    namelist->tail = &namelist->sentinel;
    namelist->len  = 0;
}

NameNode *
name_node_alloc(NameState *name_state)
{
    NameNode *name_node = name_state->first_free_name_node;

    if (name_node) {
        name_state->first_free_name_node = name_node->next;
    }
    else {
        name_node = arena_push(name_state->arena, sizeof *name_node);
    }

    return name_node;
}

void
name_node_free(NameNode *node, NameState *name_state)
{
    name_free(node->name, &name_state->name_chunk_state);
    node->next = name_state->first_free_name_node;
    name_state->first_free_name_node = node;
}

NameNode *
namelist_find(NameList *namelist, Name name)
{
    NameNode *node = namelist->sentinel.next;
    while (node) {
        if (are_name_equal(node->name, name)) {
            break;
        }
        else {
            node = node->next;
        }
    }

    return node;
}

void
namelist_push_front(NameList *namelist, Name name, NameState *name_state)
{
    NameNode *name_node = name_node_alloc(name_state);
    name_node->name = name_copy(name, &name_state->name_chunk_state);

    if (namelist->tail == &namelist->sentinel) {
        namelist->tail = name_node;
    }

    name_node->next = namelist->sentinel.next;
    namelist->sentinel.next = name_node;
    ++namelist->len;
}

void
namelist_pop_front(NameList *namelist, NameState *name_state)
{
    if (namelist->sentinel.next == NULL) {
        return;
    }

    if (namelist->tail == namelist->sentinel.next) {
        namelist->tail = &namelist->sentinel;
    }

    NameNode *node = namelist->sentinel.next;

    namelist->sentinel.next = node->next;
    name_node_free(node, name_state);
    --namelist->len;
}

void
namelist_pop(NameList *namelist, Name name, NameState *name_state)
{
    NameNode *node = &namelist->sentinel;
    while (node->next) {
        if (are_name_equal(node->next->name, name)) {
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
        name_node_free(node->next, name_state);
        node->next = node->next->next;
        --namelist->len;
    }
}

void
namelist_delete(NameList *namelist, NameState *name_state)
{
    while (namelist->sentinel.next) {
        namelist_pop_front(namelist, name_state);
    }
}

#endif // NAMELIST_C
