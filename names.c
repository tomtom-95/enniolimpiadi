#ifndef NAMES_V2
#define NAMES_V2

#include "utils.c"
#include "arena.c"
#include "string.c"

#define NAME_CHUNK_PAYLOAD_SIZE 56

typedef struct NameChunk NameChunk;
struct NameChunk {
    u8 str[NAME_CHUNK_PAYLOAD_SIZE];
    NameChunk *next;
};

typedef struct NameChunkList NameChunkList;
struct NameChunkList {
    NameChunk *head;
    u64 len;
};

typedef struct Names Names;
struct Names {
    Arena *arena; 
    NameChunk *first_free;
};

NameChunkList
name_save(Names *names, String str) {
    u64 bytes_left = str.len;
    u64 needed_chunks = (str.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;

    NameChunkList name = {.head = NULL, .len = bytes_left};
    NameChunk **chunk = &(name.head);
    for (u64 i = 0; i < needed_chunks; ++i) {
        *chunk = names->first_free;
        if (*chunk) {
            names->first_free = names->first_free->next;
        }
        else {
            *chunk = (NameChunk *)arena_push(names->arena, sizeof(NameChunk)); 
        }
        u64 bytes_to_copy = getmin((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy((*chunk)->str, str.str + i * NAME_CHUNK_PAYLOAD_SIZE, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        chunk = &((*chunk)->next);
    }
    *chunk = NULL;
    return name;
}

void
name_delete(Names *names, NameChunkList name) {
    NameChunk **chunk = &(name.head);
    while (*chunk) {
        chunk = &((*chunk)->next);
    }
    *chunk = names->first_free;
    names->first_free = name.head;
}

String
name_cat(Arena *arena, NameChunkList name) {
    u64 bytes_left = name.len;
    u64 needed_chunks = (bytes_left + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;
    String str = {.str = arena_push(arena, bytes_left), .len = bytes_left};
    NameChunk *chunk = name.head;
    for (u64 i = 0; i < needed_chunks; ++i) {
        u64 bytes_to_copy = getmin((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        memcpy(str.str + i * NAME_CHUNK_PAYLOAD_SIZE, chunk->str, bytes_to_copy);
        bytes_left -= bytes_to_copy;
        chunk = chunk->next;
    }
    return str;
}

#endif // NAMES_V2
