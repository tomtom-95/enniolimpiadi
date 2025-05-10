#ifndef NAMES_C
#define NAMES_C

#include <stdlib.h>
#include <stdio.h>

#include "utils.c"
#include "arena.c"
#include "string.c"

typedef struct Name Name;
typedef struct NameList NameList;

// always have head as a dummy first element
struct NameList {
    Name *head;
    Name *first_free_entry;
};

struct Name {
    String name;
    Name *next;
};

NameList *
name_list_init(Arena *arena) {
    NameList *ll = arena_push(arena, sizeof(*ll));
    memset(ll, 0, sizeof(*ll));

    ll->head = arena_push(arena, sizeof(*(ll->head)));
    memset(ll->head, 0, sizeof(*(ll->head)));

    return ll;
}

void
name_list_push_left(Arena *arena, NameList *ll, String name) {
    Name *node = ll->first_free_entry;

    if (node) {
        ll->first_free_entry = ll->first_free_entry->next;
    }
    else {
        node = arena_push(arena, sizeof(*node));
    }

    node->name = name;
    node->next = ll->head->next;
    ll->head->next = node;
}

void
name_list_push_right(Arena *arena, NameList *ll, String name) {
    /* walk the linked list and append node to the end */
    Name **node = &(ll->head->next);
    while (*node) {
        node = &((*node)->next);
    }

    *node = ll->first_free_entry;
    if (*node) {
        ll->first_free_entry = ll->first_free_entry->next;
    }
    else {
        *node = arena_push(arena, sizeof(**node));
    }
    (*node)->name = name;
    (*node)->next = NULL;
}

// TODO: test it
void
name_list_pop_right(NameList *ll) {
    Name **name = &(ll->head);
    while ((*name)->next) {
        name = &((*name)->next);
    }

    (*name)->next = ll->first_free_entry;
    ll->first_free_entry = *name;
}

void
name_list_pop(NameList *ll, String name) {
    Name **node = &(ll->head->next);
    while (*node) {
        if (string_are_equal((*node)->name, name)) {
            Name *node_to_remove = *node;

            *node = (*node)->next;

            node_to_remove->next = ll->first_free_entry;
            ll->first_free_entry = node_to_remove;

            return;
        }
        else {
            node = &((*node)->next);
        }
    }
}

#endif // NAMES
