#ifndef NAMES_C
#define NAMES_C

#include <stdlib.h>
#include <stdio.h>

#include "utils.c"
#include "arena.c"
#include "string.c"

typedef struct Name Name;
typedef struct NameList NameList;

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
    if (ll) {
        memset(ll, 0, sizeof(*ll));
    }

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
    node->next = ll->head;
    ll->head = node;
}

void
name_list_push_right(Arena *arena, NameList *ll, String name) {
    /* walk the linked list and append node to the end */
    Name **node = &(ll->head);
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

void
name_list_pop_right(NameList *ll) {
    Name *head = ll->head;
    if (head) {
        /* walk the linked list and get the address of the second to last node.next pointer */
        Name **node = &head;
        while ((*node)->next) {
            node = &((*node)->next);
        }

        /* Set last node as the start of the free list */
        Name *tmp = ll->first_free_entry;
        ll->first_free_entry = *node;
        (*node)->next = tmp;

        /* set second to last node.next pointer to NULL */
        *node = NULL;
    }
}

void
name_list_pop(NameList *ll, String name) {
    Name **node = &(ll->head);
    while (*node) {
        if (string_are_equal((*node)->name, name)) {
            Name *tmp = ll->first_free_entry;
            Name *node_to_remove = *node;

            *node = (*node)->next;

            ll->first_free_entry = node_to_remove;
            node_to_remove->next = tmp;

            return;
        }
        else {
            node = &((*node)->next);
        }
    }
}

#endif // NAMES
