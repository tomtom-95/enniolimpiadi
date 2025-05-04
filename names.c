#ifndef NAMES_C
#define NAMES_C

#include <stdint.h>
#include <string.h>

#include "tm_utils.c"
#include "tm_arena.c"

#define SMALL_STRING_MAX_LEN 63

typedef struct SmallString SmallString;

struct SmallString {
    u8 size;
    u8 str[SMALL_STRING_MAX_LEN];
};

SmallString *
small_string_from_cstring(Arena *arena, u8 *cstring) {
    // Copy a cstring into the arena
    SmallString *string = arena_push(arena, sizeof(*string));

    u64 cstring_len = cstring_length(cstring);
    assert(cstring_len < SMALL_STRING_MAX_LEN);

    memcpy(string->str, cstring, cstring_len);
    string->size = (u8)cstring_len;

    return string;
}

SmallString *
small_string_copy(Arena *arena, SmallString *str) {
    // Copy a SmallString type into the arena
    SmallString *res = arena_push(arena, sizeof(*res));

    assert(str->size < SMALL_STRING_MAX_LEN);

    memcpy(res->str, str->str, str->size);
    res->size = str->size;

    return res;
}

SmallString *
small_string_change(SmallString *string, u8 *cstring) {
    // Rewrite the string with the new one
    u64 cstring_len = cstring_length(cstring);
    assert(cstring_len < SMALL_STRING_MAX_LEN);

    memcpy(string->str, cstring, cstring_len);
    string->size = (u8)cstring_len;

    return string;
}

bool
small_string_are_equal(SmallString *a, SmallString *b) {
    if (a->size != b->size) {
        return false;
    }
    else {
        return memcmp(a->str, b->str, a->size) == 0;
    }
}

void
small_string_print(SmallString *string) {
    printf("%.*s\n", string->size, string->str);
}

#endif // NAMES_C
