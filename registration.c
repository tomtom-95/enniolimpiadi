#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "utils.c"
#include "names.c"
#include "string.c"
#include "registration.h"

void
registration_map_init(Arena *arena, RegistrationMap *registration_map, u64 bucket_count) {
    registration_map->registrations = arena_push(arena, sizeof(Registration *) * bucket_count);
    registration_map->bucket_count = bucket_count;
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

Registration *
registration_alloc(RegistrationState *registration_state)
{
    Registration *registration = registration_state->first_free;
    if (registration) {
        registration_state->first_free = registration_state->first_free->next;
    }
    else {
        registration = arena_push(registration_state->arena, sizeof(Registration));
    }
    memset(registration, 0, sizeof(Registration));

    return registration;
}

Registration *
registration_find(RegistrationMap *registration_map, String str)
{
    u64 bucket_num = hash_string(str) % registration_map->bucket_count;
    Registration *node = registration_map->registrations[bucket_num];

    Temp temp = scratch_get(0, 0); 

    NameState name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState name_chunk_state = {.arena = temp.arena, .first_free = NULL };
    Name *name = name_alloc(str, &name_state, &name_chunk_state);

    while (node) {
        if (name_cmp(node->registration_name, name)) {
            break;
        }
        node = node->next;
    }

    scratch_release(temp);

    return node;
}

Registration *
registration_create(RegistrationMap *registration_map, String str, RegistrationState *registration_state,
    NameState *name_state, NameChunkState *name_chunk_state)
{
    assert(!registration_find(registration_map, str));

    u64 bucket_num = hash_string(str) % registration_map->bucket_count;
    Registration **registrations = &(registration_map->registrations[bucket_num]);

    Registration *registration = registration_alloc(registration_state);

    registration->next = *registrations;
    *registrations = registration;

    Name *registration_name = name_alloc(str, name_state, name_chunk_state);
    registration->registration_name = registration_name;

    return registration;
}

void
registration_delete_(RegistrationMap *primary_map, RegistrationMap *link_map, String str,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    u64 bucket_num = hash_string(str) % primary_map->bucket_count;
    Registration **registration = &(primary_map->registrations[bucket_num]);

    Temp temp = scratch_get(0, 0);

    NameState temp_name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState temp_name_chunk_state = { .arena = temp.arena, .first_free = NULL };

    Name *primary_registration_name = name_alloc(str, &temp_name_state, &temp_name_chunk_state);
    while (*registration) {
        if (name_cmp((*registration)->registration_name, primary_registration_name)) {
            break;
        }
        else {
            registration = &((*registration)->next);
        }
    }

    Registration *registration_to_remove = *registration;

    Name *link_registration_name = registration_to_remove->registration_list.first_name;
    while (link_registration_name) {
        String str_link_registration_name = push_string_from_name(temp.arena, *link_registration_name);
        Registration *link_registration = registration_find(link_map, str_link_registration_name);
        namelist_pop_by_string(&link_registration->registration_list, str, name_state, name_chunk_state);
        link_registration_name = link_registration_name->next;
    }

    scratch_release(temp);

    namelist_delete_all(&registration_to_remove->registration_list, name_state, name_chunk_state);
    name_delete(registration_to_remove->registration_name, name_state, name_chunk_state);
    registration_to_remove->registration_name = NULL; // how much do I need this?

    *registration = (*registration)->next;
    registration_to_remove->next = registration_state->first_free;
    registration_state->first_free = registration_to_remove;
}

void
player_delete(RegistrationMap *player_map, RegistrationMap *tournament_map, String player_name,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    registration_delete_(player_map, tournament_map, player_name, registration_state, name_state, name_chunk_state);
}

void
tournament_delete(RegistrationMap *player_map, RegistrationMap *tournament_map, String tournament_name,
    RegistrationState *registration_state, NameState *name_state, NameChunkState *name_chunk_state)
{
    registration_delete_(tournament_map, player_map, tournament_name, registration_state, name_state, name_chunk_state);
}

void
player_enroll(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state)
{
    Registration *player = registration_find(player_map, str_player_name);
    assert(!namelist_find(&player->registration_list, str_tournament_name));

    Registration *tournament = registration_find(tournament_map, str_tournament_name);

    namelist_append_left(&player->registration_list, str_tournament_name, name_state, name_chunk_state);
    namelist_append_right(&tournament->registration_list, str_player_name, name_state, name_chunk_state);
}

void
player_withdraw(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state)
{
    Registration *player = registration_find(player_map, str_player_name);
    Registration *tournament = registration_find(tournament_map, str_tournament_name);

    namelist_pop_by_string(&player->registration_list, str_tournament_name, name_state, name_chunk_state);
    namelist_pop_by_string(&tournament->registration_list, str_player_name, name_state, name_chunk_state);
}

void
registration_rename_(RegistrationMap *primary_map, RegistrationMap *link_map, String *oldstr,
    String *newstr, NameState *name_state, NameChunkState *name_chunk_state)
{
    Name *newname = name_alloc(*newstr, name_state, name_chunk_state);

    Registration *primary_registration = registration_find(primary_map, *oldstr);
    assert(primary_registration);

    name_delete(primary_registration->registration_name, name_state, name_chunk_state);
    primary_registration->registration_name = newname;

    Temp temp = scratch_get(0, 0);

    Name *link_registration_name = primary_registration->registration_list.first_name;
    while (link_registration_name) {
        String str_link_registration_name = push_string_from_name(temp.arena, *link_registration_name);
        Registration *link_registration = registration_find(link_map, str_link_registration_name);
        namelist_pop_by_string(&link_registration->registration_list, *oldstr, name_state, name_chunk_state);
        namelist_append_left(&link_registration->registration_list, *newstr, name_state, name_chunk_state);
        link_registration_name = link_registration_name->next;
    }

    temp_end(temp);
}

void
player_rename(RegistrationMap *player_map, RegistrationMap *tournament_map, String *oldstr,
    String *newstr, NameState *name_state, NameChunkState *name_chunk_state)
{
    registration_rename_(player_map, tournament_map, oldstr, newstr, name_state, name_chunk_state);
}

void
tournament_rename(RegistrationMap *player_map, RegistrationMap *tournament_map, String *oldstr,
    String *newstr, NameState *name_state, NameChunkState *name_chunk_state)
{
    registration_rename_(tournament_map, player_map, oldstr, newstr, name_state, name_chunk_state);
}

StringList
list_registrations(Arena *arena, RegistrationMap *registration_map) {
    StringList string_list = {0};
    for (u64 idx = 0; idx < registration_map->bucket_count; ++idx) {
        Registration *registration = registration_map->registrations[idx]; 
        while (registration) {
            String str = push_string_from_name(arena, *(registration->registration_name));
            string_list_push(arena, &string_list, str);
            registration = registration->next;
        }
    }
    return string_list;
}

#endif // REGISTRATION_C
