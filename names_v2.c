#ifndef NAMES_V2
#define NAMES_V2

#include "utils.c"
#include "arena.c"
#include "string_v2.c"

#define NAME_CHUNK_PAYLOAD_SIZE 56

typedef struct NameChunk NameChunk;
struct NameChunk {
    u8 str[NAME_CHUNK_PAYLOAD_SIZE];
    NameChunk *next;
};

typedef struct NameChunkList NameChunkList;
struct NameChunkList {
    NameChunk *head;
};

typedef struct Names Names;
struct Names {
    Arena *arena; 
    NameChunk *first_free;
};

NameChunkList
name_write(Names *names, String str) {
    u64 bytes_left = str.len;
    u64 needed_chunks = (str.len + NAME_CHUNK_PAYLOAD_SIZE - 1) / NAME_CHUNK_PAYLOAD_SIZE;

    NameChunkList name_chunk_list = {0};
    NameChunk **name_chunk = &(name_chunk_list.head);
    for (u64 i = 0; i < needed_chunks; ++i) {
        *name_chunk = names->first_free;
        if (*name_chunk) {
            names->first_free = names->first_free->next;
        }
        else {
            *name_chunk = (NameChunk *)arena_push(names->arena, sizeof(NameChunk)); 
        }
        u64 bytes_to_copy = getmin((u64)NAME_CHUNK_PAYLOAD_SIZE, bytes_left);
        bytes_left -= bytes_to_copy;
        memcpy((*name_chunk)->str, str.str + i * NAME_CHUNK_PAYLOAD_SIZE, bytes_to_copy);
        name_chunk = &((*name_chunk)->next);
    }
    return name_chunk_list;
}

#endif // NAMES_V2
