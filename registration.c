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
    registration_enroll_(player_map, tournament_map, str_player_name,
        str_tournament_name, name_state, name_chunk_state);
}

void
tournament_enroll(RegistrationMap *player_map, RegistrationMap *tournament_map, String str_player_name,
    String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state)
{
    registration_enroll_(tournament_map, player_map, str_tournament_name,
        str_player_name, name_state, name_chunk_state);
}

void
registration_enroll_(RegistrationMap *primary_map, RegistrationMap *link_map, String primary_str,
    String link_str, NameState *name_state, NameChunkState *name_chunk_state)
{
    Registration *primary_registration = registration_find(primary_map, primary_str);
    assert(!namelist_find(&primary_registration->registration_list, link_str));

    Registration *link_registration = registration_find(link_map, link_str);
    assert(link_registration);

    namelist_append_left(&primary_registration->registration_list, link_str, name_state, name_chunk_state);
    namelist_append_left(&link_registration->registration_list, primary_str, name_state, name_chunk_state);
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
registration_withdraw_(RegistrationMap *primary_map, RegistrationMap *link_map, String primary_str,
    String link_str, NameState *name_state, NameChunkState *name_chunk_state)
{
    Registration *primary_registration = registration_find(primary_map, primary_str);
    Registration *link_registration = registration_find(link_map, link_str);

    namelist_pop_by_string(&primary_registration->registration_list, link_str, name_state, name_chunk_state);
    namelist_pop_by_string(&link_registration->registration_list, primary_str, name_state, name_chunk_state);
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

StringList
list_registrations_by_str(Arena *arena, RegistrationMap *registration_map, String str) {
    // list all the Registration that str is registered to
    // e.g. when 'registration_map' is the players map and 'str' is a player name
    // the function returs a list of all the tournaments the player is enrolled into

    Temp temp = scratch_get(&arena, 1);

    NameState temp_name_state = { .arena = temp.arena, .first_free = NULL };
    NameChunkState temp_name_chunk_state = { .arena = temp.arena, .first_free = NULL };
    Name *name = name_alloc(str, &temp_name_state, &temp_name_chunk_state);

    Registration *registration = registration_find(registration_map, str);
    while (!name_cmp(registration->registration_name, name)) {
        registration = registration->next;
    }

    StringList string_list = {0};
    Name *linkname = registration->registration_list.first_name;
    while (linkname) {
        String linkstr = push_string_from_name(arena, *linkname);
        string_list_push(arena, &string_list, linkstr);
        linkname = linkname->next;
    }

    scratch_release(temp);

    return string_list;
}

StringList
list_missing_registrations_by_str_(Arena *arena, RegistrationMap *primary_map,
    RegistrationMap *link_map, String str)
{
    // e.g. when str is a player name, list all the tournament the player is NOT enrolled into
    StringList result = {0};
    StringList link_registration_all = list_registrations(arena, link_map);
    StringList link_registration_partial = list_registrations_by_str(arena, primary_map, str); 

    StringNode *linkstr = link_registration_all.head;
    bool found = false;
    while (linkstr) {
        StringNode *linkstrpartial = link_registration_partial.head;

        while (linkstrpartial) {
            if (string_cmp(linkstr->str, linkstrpartial->str)) {
                found = true;
                break;
            }
            else {
                linkstrpartial = linkstrpartial->next;
            }
        }

        if (!found) {
            string_list_push(arena, &result, linkstr->str);
        }

        found = false;
        linkstr = linkstr->next;
    }

    return result;
}

StringList
list_missing_tournaments_by_str(Arena *arena, RegistrationMap *player_map,
    RegistrationMap *tournament_map, String str_player_name)
{
    // list all the tournaments the str_player_name is NOT enrolled into
    StringList result = list_missing_registrations_by_str_(arena, player_map, tournament_map, str_player_name);
    return result;
}

StringList
list_missing_players_by_str(Arena *arena, RegistrationMap *player_map,
    RegistrationMap *tournament_map, String str_tournament_name)
{
    // list all the players that are NOT enrolled into str_tournament_name
    StringList result = (
        list_missing_registrations_by_str_(arena, tournament_map, player_map, str_tournament_name)
    );
    return result;
}

#endif // REGISTRATION_C
