#ifndef STRING_C
#define STRING_C

#include <stdint.h>
#include <string.h>

#include "utils.c"
#include "arena.c"

#define STRING_MAX_LEN 62

typedef struct String String;

struct String {
    u8 size;
    u8 str[STRING_MAX_LEN+1];
};

u8
cstring_len(u8 *cstring) {
    u8 *p;
    for (p = cstring; *p != '\0'; p++);
    u64 len = (u64)(p - cstring);
    assert(len <= STRING_MAX_LEN);
    return (u8)len;
}

String
string_from_cstring(u8 *cstring) {
    String string;
    u64 len = cstring_len(cstring);
    assert(len <= STRING_MAX_LEN);
    memcpy(string.str, cstring, len+1);
    string.size = (u8)len;
    return string;
}

void
string_copy(String *dst, String *src) {
    assert(dst->size <= STRING_MAX_LEN);
    assert(src->size <= STRING_MAX_LEN);
    memcpy(dst, src, sizeof(*dst));
}

void
string_concat(String *dst, String *src) {
    assert(dst->size <= STRING_MAX_LEN);
    assert(src->size <= STRING_MAX_LEN);
    u64 len = dst->size + src->size; // NOTE: integer promotion
    assert(len <= STRING_MAX_LEN);
    memcpy(dst->str + dst->size, src->str, src->size + 1);
    dst->size = dst->size + src->size;
}

String *
string_change(String *string, u8 *cstring) {
    u8 len = cstring_len(cstring);
    assert(len <= STRING_MAX_LEN);
    memcpy(string->str, cstring, len+1);
    string->size = len;
    return string;
}

bool
string_are_equal(String a, String b) {
    assert(a.size <= STRING_MAX_LEN);
    assert(b.size <= STRING_MAX_LEN);
    return (
        a.size == b.size &&
        memcmp(a.str, b.str, a.size) == 0
    );
}

void
string_print(String string) {
    assert(string.size <= STRING_MAX_LEN);
    printf("%.*s\n", string.size, string.str);
}

#endif // STRING_C
