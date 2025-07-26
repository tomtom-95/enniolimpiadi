#include "utils.c"
#include "arena.c"
#include "names.c"

typedef struct Element Element;
struct Element {
    u64 data;
    u64 next;
};

typedef struct HashMap HashMap;
struct HashMap {
    u64 *index_array;
    Element *collision_array;
    u64 offset;
    u64 first_free;
    u64 bucket_count;
};

u64
hash_integer(u64 data)
{
    return (uint64_t)data * 2654435761u;
}

HashMap
hashmap_init(Arena *arena, u64 bucket_count)
{
    return (HashMap){ 
        .index_array = arena_push(arena, sizeof(u64) * bucket_count),
        .collision_array = arena_push(arena, MegaByte(1)),
        .offset = 0, .first_free = 0, .bucket_count = bucket_count
    };
}

u64
hashmap_find(HashMap *hash_map, u64 data)
{
    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 idx = hash_map->index_array[bucket];
    while (idx != 0) {
        if (hash_map->collision_array[idx].data == data) {
            return idx;
        }
        else {
            idx = hash_map->collision_array[idx].next;
        }
    }

    return idx;
}

void
hashmap_insert(HashMap *hash_map, u64 data)
{
    // make sure data is not already in the map
    assert(hashmap_find(hash_map, data) == 0);

    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 free_slot = hash_map->first_free;
    if (free_slot) {
        hash_map->first_free = hash_map->collision_array[hash_map->first_free].next;
    }
    else {
        free_slot = ++hash_map->offset;
    }

    u64 idx = hash_map->index_array[bucket];

    hash_map->index_array[bucket] = free_slot;
    hash_map->collision_array[free_slot].data = data;
    hash_map->collision_array[free_slot].next = idx;
}

void
hashmap_pop(HashMap *hash_map, u64 data)
{
    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 *idx_ptr = &hash_map->index_array[bucket];
    while (*idx_ptr != 0) {
        u64 idx = *idx_ptr;
        if (hash_map->collision_array[idx].data == data) {
            *idx_ptr = hash_map->collision_array[idx].next;

            hash_map->collision_array[idx].next = hash_map->first_free;
            hash_map->first_free = idx;

            return;
        }
        else {
            idx_ptr = &hash_map->collision_array[idx].next;
        }
    }
}

// What changed if I want to use this map for my case? data in element must not be u64
// but a Tournament data structure with all the info I actually need to store for tournament

typedef struct Tournament Tournament;
struct Tournament {
    Name name;
    NameList players_enrolled;
    u64 next;
};

typedef struct TournamentMap TournamentMap;
struct TournamentMap {
    u64 *index_array;
    Tournament *tournaments;
    u64 offset;
    u64 first_free;
    u64 bucket_count;
};

u64
hash_string(String str)
{
    u64 hash = 5381;
    for (u64 i = 0; i < str.len; ++i) {
        hash = ((hash << 5) + hash) + (u64)((str.str)[i]);
    }
    return hash;
}

TournamentMap
tournament_map_init(Arena *arena, u64 bucket_count)
{
    return (TournamentMap){ 
        .index_array = arena_push(arena, sizeof(u64) * bucket_count),
        .tournaments = arena_push(arena, MegaByte(1)),
        .offset = 0, .first_free = 0, .bucket_count = bucket_count
    };
}

u64
tournament_find(TournamentMap *map, Name tournament_name)
{
    Temp temp = scratch_get(0, 0);

    String tournament_string = string_from_name(temp.arena, tournament_name);
    u64 bucket = hash_string(tournament_string) % map->bucket_count;

    scratch_release(temp);

    u64 idx = map->index_array[bucket];
    while (idx != 0) {
        if (are_name_equal(&map->tournaments[idx].name, &tournament_name)) {
            return idx;
        }
        else {
            idx = map->tournaments[idx].next;
        }
    }

    return idx;
}

void
tournament_add(TournamentMap *map, Name tournament_name, NameChunkState *name_chunk_state)
{
    // make sure data is not already in the map
    assert(tournament_find(map, tournament_name) == 0);

    Temp temp = scratch_get(0, 0);

    String tournament_string = string_from_name(temp.arena, tournament_name);
    u64 bucket = hash_string(tournament_string) % map->bucket_count;

    scratch_release(temp);

    u64 free_slot = map->first_free;
    if (free_slot) {
        map->first_free = map->tournaments[map->first_free].next;
    }
    else {
        free_slot = ++map->offset;
    }

    u64 idx = map->index_array[bucket];

    map->index_array[bucket] = free_slot;

    name_copy(&map->tournaments[free_slot].name, &tournament_name, name_chunk_state);
    namelist_init(&map->tournaments[free_slot].players_enrolled);
    map->tournaments[free_slot].next = idx;
}

void
tournament_del(TournamentMap *map, Name tournament_name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Temp temp = scratch_get(0, 0);

    String tournament_string = string_from_name(temp.arena, tournament_name);
    u64 bucket = hash_string(tournament_string) % map->bucket_count;

    scratch_release(temp);

    u64 *idx_ptr = &map->index_array[bucket];
    while (*idx_ptr != 0) {
        u64 idx = *idx_ptr;
        if (are_name_equal(&map->tournaments[idx].name, &tournament_name)) {
            *idx_ptr = map->tournaments[idx].next;

            name_release(map->tournaments[idx].name, name_chunk_state);
            namelist_delete(&map->tournaments[idx].players_enrolled, name_state, name_chunk_state);

            map->tournaments[idx].next = map->first_free;
            map->first_free = idx;

            return;
        }
        else {
            idx_ptr = &map->tournaments[idx].next;
        }
    }
}

void
tournament_player_withdraw(TournamentMap *map, Name tournament_name, Name player_name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    u64 idx = tournament_find(map, tournament_name);

    Tournament *tournament = map->tournaments + idx;
    namelist_pop(&tournament->players_enrolled, &player_name, name_state, name_chunk_state);
}

void
tournament_player_enroll(TournamentMap *map, Name tournament_name, Name player_name,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    u64 idx = tournament_find(map, tournament_name);

    Tournament *tournament = map->tournaments + idx;
    namelist_push_front(&tournament->players_enrolled, player_name, name_state, name_chunk_state);
}