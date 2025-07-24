#ifndef LINKEDLIST_C
#define LINKEDLIST_C

#include <stdlib.h>

#include "arena.c"

// I am still too insecure about this stuff, implement it all!!!!
// When you will have it all it will become clear what are the tradeoffs

// SLL with head and tail sentinel
// very easy push and pop front

typedef struct Node Node;
struct Node {
    int data;
    Node *next;
};

typedef struct NodeState NodeState;
struct NodeState {
    Arena *arena;
    Node *first_free;
};

// better name it SLLStack
typedef struct SLLStack SLLStack;
struct SLLStack {
    Node head; 
    Node tail;
};

void
sll_init(SLLStack *ll)
{
    ll->head.next = &ll->tail;
    ll->tail.next = NULL;
}

Node *
node_alloc(NodeState *node_state)
{
    Node *node = node_state->first_free;
    if (node) {
        node_state->first_free = node_state->first_free->next;
    }
    else {
        node = arena_push(node_state->arena, sizeof *node);
    }

    return node;
}

void
node_release(Node *node, NodeState *node_state)
{
    node->next = node_state->first_free;
    node_state->first_free = node;
}

void
sll_push_front(SLLStack *ll, NodeState *node_state, int data)
{
    Node *node = node_alloc(node_state);

    node->data = data;
    node->next = ll->head.next;
    ll->head.next = node;
}

void
sll_pop_front(SLLStack *ll, NodeState *node_state)
{
    Node *node = ll->head.next;
    if (node != &ll->tail) {
        ll->head.next = node->next;
        node_release(node, node_state);
    }
}

#endif
