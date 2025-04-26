#ifndef TMHASHMAP_C
#define TMHASHMAP_C

#include <stdint.h>

#include "log.c"

#include "tm_utils.c"
#include "tm_arena.c"
#include "tm_pool.c"
#include "test_tm_linkedlistv2.c"
#include "tm_string.c"

typedef struct HashMap HashMap;
typedef struct Bucket Bucket;
typedef struct BucketNode BucketNode;

struct HashMap {
    u64 bucket_count;
    Bucket *buckets;
};

struct Bucket {
    LinkedListV2 *nodes;
};

HashMap
hashmap_init(Arena *arena, u64 bucket_count)
{
    HashMap hash_map = {0};

    hash_map.buckets = arena_push(arena, sizeof(Bucket)*bucket_count);
    if (hash_map.buckets) {
        hash_map.bucket_count = bucket_count;
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
hashmap_add(Arena *arena, HashMap hash_map, String string) {
    u64 bucket_num = choose_bucket(
        hash_string(string), hash_map.bucket_count
    );
    log_info("string.str: %s, bucket_num: %lu", string.str, bucket_num);

    Bucket *bucket = hash_map.buckets + bucket_num;

    if (!bucket->ll) {
        linked_list_v2_init(arena, string);
    }
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
