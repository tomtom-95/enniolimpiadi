#ifndef TMHASHMAP_C
#define TMHASHMAP_C

#include <stdint.h>

#include "log.c"

#include "tm_utils.c"
#include "tm_arena.c"
#include "tm_string.c"

typedef struct HashMap HashMap;
typedef struct BucketNode BucketNode;
typedef struct BucketState BucketState;
typedef struct BucketNodeState BucketNodeState;

struct HashMap {
    u64 bucket_count;
    BucketState *bucket_state;
};

struct BucketState {
    BucketNode *first_bucket_node;
    BucketNode *first_free_entry;
};

struct BucketNode {
    String string;
    BucketNode *next;
    BucketNodeState *node_state;
};

struct BucketNodeState {
    void *first_data;
    void *first_free_entry;
};


HashMap
hashmap_init(Arena *arena, u64 bucket_count)
{
    HashMap hash_map = {0};

    BucketState *bucket_states = (
        arena_push(
            arena,
            sizeof(BucketState)*bucket_count
        ) /* is it 0 initialized by default? */
    );

    if (bucket_states) {
        hash_map.bucket_count = bucket_count;
        hash_map.bucket_state = bucket_states;
    }
    else {
        log_error("HashMap allocation failed");
    }

    return hash_map;
}

u64
hash_string(String string)
{
    u64 hash = 5381;

    for (u64 i = 0; i < string.size; i++) {
        hash = (
            // hash * 33 + c
            ((hash << 5) + hash) + (u64)string.str[i]
        );
    }
    
    return hash;
}

u64
choose_bucket(u64 hash, u64 bucket_count)
{
    return hash % bucket_count;
}

u8
hashmap_add(
    Arena *arena,
    HashMap hash_map,
    String string
)
{
    u64 bucket_num = choose_bucket(
        hash_string(string), hash_map.bucket_count
    );
    log_info("string.str: %s, bucket_num: %lu", string.str, bucket_num);

    BucketState *bucket_state = hash_map.bucket_state + bucket_num;

    BucketNode **bucket_node = &(bucket_state->first_bucket_node);
    BucketNode *first_free_entry = bucket_state->first_free_entry;

    /* Check if in the bucket there is already a Node that uses string */
    while (*bucket_node != NULL) {
        if (tmstring_are_equal((*bucket_node)->string, string)) {
            log_error(
                "String %s is already registered in the hashmap. "
                "Cannot register two bucket with the same string",
                string.str
            );
            return 1;
        } 
        else {
            bucket_node = &((*bucket_node)->next);
        }
    }

    /* Grab a node from the free list, else push a new node into the arena */
    if (first_free_entry) {
        *bucket_node = first_free_entry;
        first_free_entry = first_free_entry->next;
    }
    else {
        *bucket_node = arena_push(arena, sizeof(BucketNode));
    }

    (*bucket_node)->string = string;
    (*bucket_node)->data = NULL;

    return 0;
}

BucketNode *
hashmap_find(
    String string,
    HashMap hash_map
) {
    u64 bucket_num = choose_bucket(
        hash_string(string), hash_map.bucket_count
    );

    BucketState *bucket_state = hash_map.bucket_state + bucket_num;

    BucketNode **bucket_node = &(bucket_state->first_bucket_node);

    /* Check if in the bucket there is a Node that uses string */
    while (*bucket_node != NULL) {
        if (tmstring_are_equal((*bucket_node)->string, string)) {
            return *bucket_node;
        } 
        else {
            bucket_node = &((*bucket_node)->next);
        }
    }

    /* string not found */
    return NULL;
}

#endif // TMHASHMAP_C
