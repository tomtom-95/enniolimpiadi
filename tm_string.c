#ifndef TM_STRING_C
#define TM_STRING_C

#include <stdint.h>
#include <string.h>

#include "tm_utils.c"
#include "tm_arena.c"

typedef struct {
    u8 *str;
    u64 size;
} String;

#define str_lit(S)  tmstring((u8*)(S), sizeof(S) - 1)
#define str_lit_comp(S) {(u8*)(S), sizeof(S) - 1,}

u64
cstring_length(u8 *c) {
  u8 *p = c;
  while (*p != 0) {
    p++;
  }
  return (u64)(p - c);
}

String
tmstring(u8 *string, u64 size) {
    String res = {.str = string, .size = size};
    return res;
}

String
tmstring_arena_write(Arena *arena, String string) {
    u8 *start = arena_push(arena, string.size + 1);
    memcpy(start, string.str, string.size);
    start[string.size] = 0;
    
    String str = {.str = start, .size = string.size};
    return str;
}

/* allocation and initialization */
String
tmstring_alloc_and_init(u8 *cstring) {
    u64 size = cstring_length(cstring);

    u8 *str = malloc(size);
    memcpy(str, cstring, size);

    String string = {.size = size, .str = str};
    return string;
}

void
tmstring_realloc_and_reinit(String *string, u8 *cstring) {
    u64 size = cstring_length(cstring);

    string->str = realloc(string->str, size);
    memcpy(string->str, cstring, size);
}

void
tmstring_free(String *string) {
    free(string->str);
}

bool
tmstring_are_equal(String a, String b) {
    if (a.size != b.size) {
        return false;
    }
    else {
        for (u64 i = 0; i < a.size; i++) {
            if (a.str[i] != b.str[i]) {
                return false;
            }
        }
        return true;
    }
}

#endif // TM_STRING_C
