#ifndef TM_LINKEDLISTV2
#define TM_LINKEDLISTV2

#include "tm_utils.c"
#include "tm_pool.c"

typedef struct LinkedListV2 LinkedListV2;
typedef struct Node Node;

struct LinkedListV2 {
    Pool *pool;
    Node *head;
};

struct Node {
    String data;

    // I should put something like this in the data, when a new node is allocated, a new arena is spawned for that specific node.
    // When the node is deallocated trough the linked list removal, I must also free the tournaments arena.
    // void *tournaments 
    Node *next;
};

LinkedListV2
linked_list_v2_init(Arena *arena, String data) {
    LinkedListV2 ll = {0};

    ll.pool = pool_init(arena, sizeof(Node));
    ll.head = pool_alloc(arena, ll.pool);

    (ll.head)->data = data;
    (ll.head)->next = NULL;

    return ll;
}

LinkedListV2
linked_list_v2_push_left(Arena *arena, LinkedListV2 ll, String data) {
    Node *node = pool_alloc(arena, ll.pool);

    node->data = data;

    node->next = ll.head;
    ll.head = node;

    return ll;
}

LinkedListV2
linked_list_v2_push_right(Arena *arena, LinkedListV2 ll, String data) {
    Node **node = &(ll.head);
    while (*node) {
        node = &((*node)->next);
    }
    *node = pool_alloc(arena, ll.pool);
    (*node)->data = data;
    (*node)->next = NULL;

    return ll;
}

LinkedListV2
linked_list_v2_pop_right(LinkedListV2 ll) {
    if (ll.head) {
        Node **node = &(ll.head);
        while ((*node)->next) {
            node = &((*node)->next);
        }
        pool_free(ll.pool, (PoolFreeNode *)(*node));
        *node = NULL;
    }

    return ll;
}

#endif // TM_LINKEDLISTV2
