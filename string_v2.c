#ifndef STRING_V2_C
#define STRING_V2_C

#include <stdbool.h>
#include <string.h>

#include "utils.c"
#include "arena.c"

typedef struct {
    u8 *str;
    u64 len;
} String;

#define cstring_lit_len(string) ArrayCount(string)
#define string_from_cstring_lit(string) string_from_cstring((u8 *)string)

u64
cstring_len(u8 *cstring) {
    u8 *p = cstring;
    while (*p++);
    return (u64)(p - cstring);
}

void
cstring_cpy(u8 *dst, u8 *src) {
    while ((*dst++ = *src++));
}

String
string_from_cstring(u8 *cstring) {
    return (String){.str = cstring, .len = cstring_len(cstring)};
}

String
string_cpy(Arena *arena, String src) {
    u64 len = src.len;
    String str = {
        .str = arena_push(arena, src.len),
        .len = len
    };
    cstring_cpy(str.str, src.str);
    return str;
}

String
string_cat(Arena *arena, String s1, String s2) {
    u64 len = s1.len + s2.len - 1;
    String str = {
        .str = arena_push(arena, len),
        .len = len
    };
    cstring_cpy(str.str, s1.str);
    cstring_cpy(str.str + s1.len - 1, s2.str);
    return str;
}

bool
string_cmp(String s1, String s2) {
    return s1.len == s2.len && (memcmp(s1.str, s2.str, s1.len) == 0); 
}

#endif // STRING_V2_C
