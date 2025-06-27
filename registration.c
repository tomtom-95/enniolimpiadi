#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "utils.c"
#include "names.c"
#include "string.c"
#include "registration.h"

void
player_map_init(Arena *arena, PlayerMap *player_map, u64 bucket_count)
{
    player_map->players = arena_push(arena, sizeof(Player *) * bucket_count);
    player_map->bucket_count = bucket_count;
}

void
tournament_map_init(Arena *arena, TournamentMap *tournament_map, u64 bucket_count)
{
    tournament_map->tournaments = arena_push(arena, sizeof(Tournament *) * bucket_count);
    tournament_map->bucket_count = bucket_count;
}

u64
hash_string(String str)
{
    u64 hash = 5381;
    for (u64 i = 0; i < str.len; ++i) {
        hash = ((hash << 5) + hash) + (u64)((str.str)[i]);
    }
    return hash;
}

Player *
player_alloc(PlayerState *player_state)
{
    Player *player = player_state->first_free;
    if (player) {
        player_state->first_free = player_state->first_free->next;
    }
    else {
        player = arena_push(player_state->arena, sizeof(Player)); 
    }
    memset(player, 0, sizeof(Player));

    return player;
}

Tournament *
tournament_alloc(TournamentState *tournament_state)
{
    Tournament *tournament = tournament_state->first_free;
    if (tournament) {
        tournament_state->first_free = tournament_state->first_free->next;
    }
    else {
        tournament = arena_push(tournament_state->arena, sizeof(Tournament));
    }
    memset(tournament, 0, sizeof(Tournament));

    return tournament;
}

Player *
player_find(PlayerMap *player_map, String str_player_name)
{
    u64 player_bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player *player_node = player_map->players[player_bucket_num];

    Temp temp = scratch_get(0, 0); 

    NameState name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState name_chunk_state = {.arena = temp.arena, .first_free = NULL };
    Name *player_name = name_alloc(str_player_name, &name_state, &name_chunk_state);

    while (player_node) {
        if (name_cmp(player_node->player_name, player_name)) {
            break;
        }
        player_node = player_node->next;
    }

    scratch_release(temp);

    return player_node;
}

Tournament *
tournament_find(TournamentMap *tournament_map, String str_tournament_name)
{
    u64 tournament_bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;
    Tournament *tournament_node = tournament_map->tournaments[tournament_bucket_num];

    Temp temp = scratch_get(0, 0); 

    NameState name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState name_chunk_state = {.arena = temp.arena, .first_free = NULL };
    Name *tournament_name = name_alloc(str_tournament_name, &name_state, &name_chunk_state);

    while (tournament_node) {
        if (name_cmp(tournament_node->tournament_name, tournament_name)) {
            break;
        }
        tournament_node = tournament_node->next;
    }

    scratch_release(temp);

    return tournament_node;
}

Player *
player_create(PlayerMap *player_map, String str_player_name, PlayerState *player_state,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Player *player = player_find(player_map, str_player_name);
    assert(!player);

    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **players = &(player_map->players[bucket_num]);

    Player *newplayer = player_alloc(player_state);

    newplayer->next = *players;
    *players = newplayer;

    // maybe I do not like the semantic on name_alloc, it does not just alloc, it also assign the name
    Name *player_name = name_alloc(str_player_name, name_state, name_chunk_state);
    newplayer->player_name = player_name;

    return newplayer;
}

Tournament *
tournament_create(TournamentMap *tournament_map, String str_tournament_name, TournamentState *tournament_state,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    Tournament *tournament = tournament_find(tournament_map, str_tournament_name);
    assert(!tournament);

    u64 bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;
    Tournament **tournaments = &(tournament_map->tournaments[bucket_num]);

    Tournament *newtournament = tournament_alloc(tournament_state);

    newtournament->next = *tournaments;
    *tournaments = newtournament;

    Name *tournament_name = name_alloc(str_tournament_name, name_state, name_chunk_state);
    newtournament->tournament_name = tournament_name;

    return newtournament;
}

void
player_delete(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name,
    PlayerState *player_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);

    Temp temp = scratch_get(0, 0);

    NameState temp_name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState temp_name_chunk_state = { .arena = temp.arena, .first_free = NULL };

    Name *player_name = name_alloc(str_player_name, &temp_name_state, &temp_name_chunk_state);

    while (*player) {
        if (name_cmp((*player)->player_name, player_name)) {
            break;
        }
        else {
            player = &((*player)->next);
        }
    }

    Player *player_to_remove = *player;

    // Remove the player from all the tournaments it was enrolled
    // TODO: probably can be done in one pass instead of calling namelist_delete outside of the loop
    Name *tournament_name = player_to_remove->tournament_names.first_name;
    while (tournament_name) {
        String str_tournament_name = push_string_from_name(temp.arena, *tournament_name);
        Tournament *tournament = tournament_find(tournament_map, str_tournament_name);
        namelist_pop_by_string(&tournament->player_names, str_player_name, name_state, name_chunk_state);
        tournament_name = tournament_name->next;
    }

    scratch_release(temp);

    namelist_delete_all(&player_to_remove->tournament_names, name_state, name_chunk_state);
    name_delete(player_to_remove->player_name, name_state, name_chunk_state);
    player_to_remove->player_name = NULL; // how much do I need this?

    *player = (*player)->next;
    player_to_remove->next = player_state->first_free;
    player_state->first_free = player_to_remove;
}

void
tournament_delete(PlayerMap *player_map, TournamentMap *tournament_map, String str_tournament_name,
    TournamentState *tournament_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    u64 bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;
    Tournament **tournament = &(tournament_map->tournaments[bucket_num]);

    Temp temp = scratch_get(0, 0);

    NameState temp_name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState temp_name_chunk_state = { .arena = temp.arena, .first_free = NULL };

    Name *tournament_name = name_alloc(str_tournament_name, &temp_name_state, &temp_name_chunk_state);

    while (*tournament) {
        if (name_cmp((*tournament)->tournament_name, tournament_name)) {
            break;
        }
        else {
            tournament = &((*tournament)->next);
        }
    }

    Tournament *tournament_to_remove = *tournament;
    Name *player_name = tournament_to_remove->player_names.first_name;
    while (player_name) {
        String str_player_name = push_string_from_name(temp.arena, *player_name);
        Player *player = player_find(player_map, str_player_name);
        namelist_pop_by_string(&player->tournament_names, str_tournament_name, name_state, name_chunk_state);
        player_name = player_name->next;
    }

    scratch_release(temp);

    namelist_delete_all(&tournament_to_remove->player_names, name_state, name_chunk_state);
    name_delete(tournament_to_remove->tournament_name, name_state, name_chunk_state);
    tournament_to_remove->tournament_name = NULL; // how much do I need this?

    *tournament = (*tournament)->next;
    tournament_to_remove->next = tournament_state->first_free;
    tournament_state->first_free = tournament_to_remove;
}


void
player_enroll(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, String str_tournament_name,
    PlayerState *player_state, TournamentState *tournament_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    Player *player = player_find(player_map, str_player_name);
    assert(!namelist_find(&player->tournament_names, str_tournament_name));

    Tournament *tournament = tournament_find(tournament_map, str_tournament_name);

    namelist_append_left(&player->tournament_names, str_tournament_name, name_state, name_chunk_state);
    namelist_append_right(&tournament->player_names, str_player_name, name_state, name_chunk_state);
}

void
player_withdraw(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state)
{
    Player *player = player_find(player_map, str_player_name);
    Tournament *tournament = tournament_find(tournament_map, str_tournament_name);

    namelist_pop_by_string(&player->tournament_names, str_tournament_name, name_state, name_chunk_state);
    namelist_pop_by_string(&tournament->player_names, str_player_name, name_state, name_chunk_state);
}

// void
// player_rename(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_oldname,
//     String str_player_newname, NameState *name_state, NameChunkState *name_chunk_state)
// {
//     Name *newname = name_alloc(str_player_newname, name_state, name_chunk_state);
// 
//     Player *player = player_find(player_map, str_player_oldname);
//     assert(player);
// 
//     name_delete(name_chunk_state, &(player->player_name));
//     player->player_name = newname;
// 
//     Temp temp = temp_begin(arena);
//     Name *tournament_name = player->tournament_names.head;
//     while (tournament_name) {
//         String str_tournament_name = push_string_from_name(temp.arena, *tournament_name);
//         Tournament *tournament = tournament_find(arena, tournament_map, str_tournament_name);
// 
//         Name *player_name = tournament->player_names.head;
//         while (!string_cmp(str_oldname, push_string_from_name(temp.arena, *player_name))) {
//             player_name = player_name->next;
//         }
//         *player_name = newname;
//         tournament_name = tournament_name->next;
//     }
//     temp_end(temp);
// }

StringList
list_tournaments(Arena *arena, TournamentMap *tournament_map) {
    StringList string_list = {0};
    for (u64 idx = 0; idx < tournament_map->bucket_count; ++idx) {
        Tournament *tournament = tournament_map->tournaments[idx]; 
        while (tournament) {
            String str_tournament_name = push_string_from_name(arena, *(tournament->tournament_name));
            string_list_push(arena, &string_list, str_tournament_name);
            tournament = tournament->next;
        }
    }
    return string_list;
}

#endif // REGISTRATION_C
