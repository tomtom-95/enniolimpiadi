#ifndef REGISTRATION_V2_C
#define REGISTRATION_V2_C

#include "utils.c"
#include "arena.c"
#include "names.c"

#include "registration_v2.h"

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
        if (are_name_equal(map->tournaments[idx].name, tournament_name)) {
            return idx;
        }
        else {
            idx = map->tournaments[idx].next;
        }
    }

    return idx;
}

void
tournament_add(TournamentMap *map, Name tournament_name, NameState *name_state)
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

    map->tournaments[free_slot].name = name_copy(tournament_name, &name_state->name_chunk_state);
    namelist_init(&map->tournaments[free_slot].players_enrolled);
    map->tournaments[free_slot].next = idx;
}

void
tournament_del(TournamentMap *map, Name tournament_name, NameState *name_state)
{
    Temp temp = scratch_get(0, 0);

    String tournament_string = string_from_name(temp.arena, tournament_name);
    u64 bucket = hash_string(tournament_string) % map->bucket_count;

    scratch_release(temp);

    u64 *idx_ptr = &map->index_array[bucket];
    while (*idx_ptr != 0) {
        u64 idx = *idx_ptr;
        if (are_name_equal(map->tournaments[idx].name, tournament_name)) {
            *idx_ptr = map->tournaments[idx].next;

            name_free(map->tournaments[idx].name, &name_state->name_chunk_state);
            namelist_delete(&map->tournaments[idx].players_enrolled, name_state);

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
tournament_player_enroll(TournamentMap *map, Name tournament_name,
    Name player_name, NameState *name_state)
{
    u64 idx = tournament_find(map, tournament_name);

    Tournament *tournament = map->tournaments + idx;
    namelist_push_front(&tournament->players_enrolled, player_name, name_state);
}

void
tournament_player_withdraw(TournamentMap *map, Name tournament_name,
    Name player_name, NameState *name_state)
{
    u64 idx = tournament_find(map, tournament_name);

    Tournament *tournament = map->tournaments + idx;
    namelist_pop(&tournament->players_enrolled, player_name, name_state);
}

NameList *
list_all_tournaments(TournamentMap *map, NameState *name_state, Arena *arena)
{
    NameList *namelist = arena_push(arena, sizeof *namelist);
    namelist_init(namelist);

    for (u64 i = 0; i < map->bucket_count; ++i) {
        u64 j = map->index_array[i];
        while (j != 0) {
            namelist_push_front(namelist, map->tournaments[j].name, name_state);
            j = map->tournaments[j].next;
        }
    }

    return namelist;
}

NameList *
list_all_players(TournamentMap *map, NameState *name_state, Arena *arena)
{
    NameList *namelist = arena_push(arena, sizeof *namelist);
    namelist_init(namelist);

    for (u64 i = 0; i < map->bucket_count; ++i) {
        u64 j = map->index_array[i];
        while (j != 0) {
            NameList players_enrolled = map->tournaments[j].players_enrolled;
            NameNode *player_node = &players_enrolled.sentinel;
            for (u64 k = 0; k < players_enrolled.len; ++k) {
                player_node = player_node->next;
                if (!namelist_find(namelist, player_node->name)) {
                    namelist_push_front(namelist, player_node->name, name_state);
                }
            }
            j = map->tournaments[j].next;
        }
    }

    return namelist;
}

NameList *
list_tournaments_joined_by_player(TournamentMap *map, Name player_name,
    NameState *name_state, Arena *arena)
{
    NameList *tournaments = arena_push(arena, sizeof *tournaments);
    namelist_init(tournaments);

    for (u64 i = 0; i < map->bucket_count; ++i) {
        u64 j = map->index_array[i];
        while (j != 0) {
            NameList players_enrolled = map->tournaments[j].players_enrolled;
            if (namelist_find(&players_enrolled, player_name)) {
                namelist_push_front(tournaments, map->tournaments[i].name, name_state);
            }
            j = map->tournaments[j].next;
        }
    }

    return tournaments;
}

NameList *
list_tournaments_not_joined_by_player(TournamentMap *map, Name player_name,
    NameState *name_state, Arena *arena)
{
    NameList *tournaments = arena_push(arena, sizeof *tournaments);
    namelist_init(tournaments);

    for (u64 i = 0; i < map->bucket_count; ++i) {
        u64 j = map->index_array[i];
        while (j != 0) {
            NameList players_enrolled = map->tournaments[j].players_enrolled;
            if (!namelist_find(&players_enrolled, player_name)) {
                namelist_push_front(tournaments, map->tournaments[i].name, name_state);
            }
            j = map->tournaments[j].next;
        }
    }

    return tournaments;
}

#endif // REGISTRATION_V2_C
