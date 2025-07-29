#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "utils.c"
#include "string.c"
#include "names.h"
#include "namelist.h"

typedef struct Registration Registration;
struct Registration {
    NameNode *registration_name;
    NameList registration_list;
    Registration *next;
};

typedef struct RegistrationState RegistrationState;
struct RegistrationState {
    Arena *arena;
    Registration *first_free;
};

typedef struct RegistrationMap RegistrationMap;
struct RegistrationMap {
    u64 bucket_count;
    Registration **registrations;
};

u64
hash_string(String str);

void
registration_map_init(Arena *arena, RegistrationMap *registration_map, u64 bucket_count);

Registration *
registration_alloc(RegistrationState *registration_state);

Registration *
registration_find(RegistrationMap *registration_map, String str);

Registration *
registration_create(RegistrationMap *registration_map, String str, RegistrationState *registration_state,
    NameState *name_state, NameChunkState *name_chunk_state);

void
registration_enroll_(RegistrationMap *primary_map, RegistrationMap *link_map, String primary_str,
    String link_str, NameState *name_state, NameChunkState *name_chunk_state);

void
registration_delete_(RegistrationMap *primary_map, RegistrationMap *link_map, String str,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state);

void
registration_withdraw_(RegistrationMap *primary_map, RegistrationMap *link_map, String primary_str,
    String link_str, NameState *name_state, NameChunkState *name_chunk_state);

void
player_enroll(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state);

void
player_withdraw(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state);

void
player_delete(RegistrationMap *player_map, RegistrationMap *tournament_map, String player_name,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state);

void
tournament_delete(RegistrationMap *player_map, RegistrationMap *tournament_map, String tournament_name,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state);

void
tournament_enroll(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state);

StringList
list_registrations(Arena *arena, RegistrationMap *registration_map);

StringList
list_registrations_by_str(Arena *arena, RegistrationMap *registration_map, String str);

StringList
list_missing_registrations_by_str_(Arena *arena, RegistrationMap *primary_map,
    RegistrationMap *link_map, String str);

StringList
list_missing_tournaments_by_str(Arena *arena, RegistrationMap *player_map,
    RegistrationMap *tournament_map, String str_player_name);

StringList
list_missing_players_by_str(Arena *arena, RegistrationMap *player_map,
    RegistrationMap *tournament_map, String str_tournament_name);

#endif // REGISTRATION_H
