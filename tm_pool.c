#ifndef TM_POOL_C
#define TM_POOL_C

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>

#include "log.c"

#include "tm_utils.c"
#include "tm_arena.c"

typedef struct Pool Pool;
typedef struct PoolFreeNode PoolFreeNode;

struct Pool {
    u64 node_size;
    PoolFreeNode *first_free_node;
};

struct PoolFreeNode {
    PoolFreeNode *next;
};

Pool *
pool_init(Arena *arena, u64 node_size) {
    /* TODO: assert that node_size is a power of 2 */
    assert(node_size >= sizeof(void *));
    
    log_info(
        "Allocating %" PRIu64 " bytes for the pool allocator ...",
        sizeof(Pool)
    );

    Pool *allocator = arena_push(arena, sizeof(Pool));
    if (allocator) {
        allocator->node_size = node_size;
        allocator->first_free_node = NULL;
    }
    else {
        log_error("Allocation failed.");
    }

    return allocator;
}

void *
pool_alloc(Arena *arena, Pool *allocator) {
    PoolFreeNode *res = allocator->first_free_node;
    if (res) {
        allocator->first_free_node = allocator->first_free_node->next;
    }
    else {
        res = arena_push(arena, allocator->node_size);
    }

    return res;
}

void
pool_free(Pool *allocator, PoolFreeNode *node) {
    node->next = allocator->first_free_node;
    allocator->first_free_node = node;
}

#endif // TM_POOL_C
