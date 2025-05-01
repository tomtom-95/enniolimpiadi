#ifndef TM_LINKEDLIST_C
#define TM_LINKEDLIST_C

#include <stdlib.h>
#include <stdio.h>

#include "tm_utils.c"
#include "tm_arena.c"
#include "tm_string.c"

typedef struct LinkedList LinkedList;
typedef struct Node Node;

struct LinkedList {
    Node *head;
    Node *first_free_entry;
};

struct Node {
    String data;
    Node *next;
};

LinkedList *
linked_list_init(Arena *arena) {
    LinkedList *ll = arena_push(arena, sizeof(LinkedList));
    if (ll) {
        ll->head = NULL;
        ll->first_free_entry = NULL;
    }

    return ll;
}

void
linked_list_push_left(Arena *arena, LinkedList *ll, String data) {
    Node *node = ll->first_free_entry;

    if (node) {
        ll->first_free_entry = ll->first_free_entry->next;
    }
    else {
        node = arena_push(arena, sizeof(Node));
    }

    node->data = data;
    node->next = ll->head;
    ll->head = node;
}

void
linked_list_push_right(Arena *arena, LinkedList *ll, String data) {
    /* walk the linked list and append node to the end */
    Node **node = &(ll->head);
    while (*node) {
        node = &((*node)->next);
    }

    *node = ll->first_free_entry;
    if (*node) {
        ll->first_free_entry = ll->first_free_entry->next;
    }
    else {
        *node = arena_push(arena, sizeof(Node));
    }
    (*node)->data = data;
    (*node)->next = NULL;
}

void
linked_list_pop_right(LinkedList *ll) {
    Node *head = ll->head;
    if (head) {
        /* walk the linked list and get the address of the second to last node.next pointer */
        Node **node = &head;
        while ((*node)->next) {
            node = &((*node)->next);
        }

        /* Set last node as the start of the free list */
        Node *tmp = ll->first_free_entry;
        ll->first_free_entry = *node;
        (*node)->next = tmp;

        /* set second to last node.next pointer to NULL */
        *node = NULL;
    }
}

void
linked_list_pop(LinkedList *ll, String data) {
    Node **node = &(ll->head);
    while (*node) {
        if (string_are_equal((*node)->data, data)) {
            Node *tmp = ll->first_free_entry;
            Node *node_to_remove = *node;

            *node = (*node)->next;

            ll->first_free_entry = *node;
            node_to_remove->next = tmp;

            return;
        }
        else {
            node = &((*node)->next);
        }
    }
}

#endif // TM_LINKEDLIST_C
