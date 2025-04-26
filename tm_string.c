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

#define str_lit(S)  string((u8*)(S), sizeof(S) - 1)
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
string(u8 *string, u64 size) {
    String res = {.str = string, .size = size};
    return res;
}

String
string_write(u8 *cstring) {
    u64 size = cstring_length(cstring);

    u8 *str = malloc(size);
    memcpy(str, cstring, size);

    String string = {.size = size, .str = str};
    return string;
}

String
string_replace(String string, u8 *cstring) {
    u64 size = cstring_length(cstring);

    free(string.str);
    string.str = malloc(size);

    memcpy(string.str, cstring, size);
    string.size = size;

    return string;
}

void
string_delete(String *string) {
    free(string->str);
    string->str = NULL;
    string->size = 0;
}

bool
string_are_equal(String a, String b) {
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
