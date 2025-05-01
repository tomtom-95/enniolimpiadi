#ifndef TMHASHMAP_C
#define TMHASHMAP_C

#include <stdint.h>

#include "log.c"

#include "tm_utils.c"
#include "tm_arena.c"
#include "tm_pool.c"
#include "tm_linkedlistv3.c"
#include "tm_string.c"

typedef struct HashMap HashMap;
typedef struct Bucket Bucket;
typedef struct BucketNode BucketNode;

struct HashMap {
    u64 bucket_count;
    Bucket *buckets;
};

struct Bucket {
    Pool *pool;
    BucketNode *head;
};

struct BucketNode {
    String key;
    BucketNode *next;
    LinkedListV3 *data;
};

HashMap
hashmap_init(Arena *arena, u64 bucket_count) {
    HashMap hash_map = {0};

    hash_map.buckets = arena_push(arena, sizeof(*(hash_map.buckets))*bucket_count);
    if (hash_map.buckets) {
        hash_map.bucket_count = bucket_count;
    }
    else {
        log_error("HashMap allocation failed");
    }

    for (u64 i; i < bucket_count; i++) {
        hash_map.buckets[i].pool = pool_init(arena, sizeof(BucketNode));
        hash_map.buckets[i].head = NULL;
    }

    return hash_map;
}

u64
hash_string(String string) {
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
choose_bucket(u64 hash, u64 bucket_count) {
    return hash % bucket_count;
}

u8
hashmap_add(Arena *arena, HashMap hash_map, String player_name) {
    u64 bucket_num = choose_bucket(
        hash_string(player_name), hash_map.bucket_count
    );

    Bucket *bucket = hash_map.buckets + bucket_num;

    BucketNode **bucket_node = &(bucket->head);
    while (*bucket_node) {
        if (string_are_equal((*bucket_node)->key, player_name)) {
            log_error("Hash map has already key %s", player_name);
        }
        else {
            bucket_node = &((*bucket_node)->next);
        }
    }

    *bucket_node = pool_alloc(arena, bucket->pool);
    (*bucket_node)->key = player_name;
    *(*bucket_node)->data = linked_list_v3_init(arena);
    (*bucket_node)->next = NULL;
}

Node *
hashmap_find(String key, HashMap hash_map) {
    u64 bucket_num = choose_bucket(
        hash_string(key), hash_map.bucket_count
    );

    LinkedListV3 *bucket = hash_map.buckets + bucket_num;

    return linked_list_v3_find(*bucket, &key, string_are_equal);
}

#endif // TMHASHMAP_C
