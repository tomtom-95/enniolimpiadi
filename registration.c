#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "tm_utils.c"
#include "tm_string.c"
#include "tm_linkedlist.c"

typedef struct Players Players;
typedef struct PlayerNode PlayerNode;

typedef struct Tournaments Tournaments;
typedef struct TournamentNode TournamentNode;

struct Players {
    PlayerNode *head;
    PlayerNode *first_free_entry;
};

struct Tournaments {
    TournamentNode *first_free_entry;
};

struct PlayerNode {
    String player_name;
    TournamentNode *tournament_head;
    PlayerNode *next;
};

struct TournamentNode {
    String tournament_name;
    TournamentNode *next;
};

Players *
players_init(Arena *arena) {
    Players *players = arena_push(arena, sizeof(*players));
    if (players) {
        players->head = NULL;
        players->first_free_entry = NULL;
    }

    return players;
}

Tournaments *
tournaments_init(Arena *arena) {
    Tournaments *tournaments = arena_push(arena, sizeof(*tournaments));
    if (tournaments) {
        tournaments->first_free_entry = NULL;
    }

    return tournaments;
}

void
players_add(Arena *arena, Players *players, String player_name) {
    PlayerNode *node = players->first_free_entry;

    if (node) {
        players->first_free_entry = players->first_free_entry->next;
    }
    else {
        node = arena_push(arena, sizeof(*node));
    }

    node->player_name = player_name;
    node->tournament_head = NULL; 
    node->next = players->head;

    players->head = node;
}

void
players_remove(Players *players, Tournaments *tournaments, String player_name) {
    PlayerNode **player = &(players->head);
    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            PlayerNode *tmp = players->first_free_entry;
            PlayerNode *player_to_remove = *player;

            // TODO(tommaso): Dealloc all the tournaments, this is wrong now
            TournamentNode *tmp = tournaments->first_free_entry;
            tournaments->first_free_entry = (*player)->tournament_head;


            *player = (*player)->next;

            players->first_free_entry = *player;
            player_to_remove->next = tmp;

            return;
        }
        else {
            player = &((*player)->next);
        }
    }
}

void
players_register(
    Arena *arena,
    Players *players,
    Tournaments *tournaments,
    String player_name,
    String tournament_name
) {
    PlayerNode **player = &(players->head);
    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            TournamentNode **tournament = &((*player)->tournament_head);
            while (*tournament) {
                // TODO(tommaso): check that player is not actually already registered
                tournament = &((*tournament)->next);
            }
            *tournament = tournaments->first_free_entry;
            if (*tournament) {
                tournaments->first_free_entry = tournaments->first_free_entry->next;
            }
            else {
                *tournament = arena_push(arena, sizeof(TournamentNode));
            }

            (*tournament)->tournament_name = tournament_name;
            (*tournament)->next = NULL;

            return;
        }
        else {
            player = &((*player)->next);
        }
    }

    assert(0);
}

// void
// people_unregister(
//     Players *players,
//     Tournaments *tournaments,
//     String player_name,
//     String tournament_name
// ) {
//     // deregister a person from a tournament
//     PlayerNode **player = &(players->head);
//     while (*player) {
//         if (string_are_equal((*player)->name, player_name)) {
//             linked_list_pop(
//                 (*player)->tournaments,
//                 tournament_name
//             );
// 
//             return;
//         }
//         else {
//             player = &((*player)->next);
//         }
//     }
// }

#endif // REGISTRATION_C
