#ifndef STRING_C
#define STRING_C

#include <stdbool.h>
#include <string.h>

#include "utils.c"
#include "arena.c"

typedef struct String String;
struct String {
    u8 *str;
    u64 len;
};

typedef struct StringNode StringNode;
struct StringNode {
    String str; 
    StringNode *next;
};

typedef struct StringList StringList;
struct StringList {
    StringNode *head;
    u64 len;
};

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
    return (String){.str = cstring, .len = cstring_len(cstring) - 1};
}

void
string_cpy(String *dst, String *src) {
    memcpy(dst->str, src->str, src->len);
    dst->len = src->len;
}

String
string_cat(Arena *arena, String s1, String s2) {
    u64 len = s1.len + s2.len;
    String str = {.str = arena_push(arena, len), .len = len};
    memcpy(str.str, s1.str, s1.len);
    memcpy(str.str + s1.len, s2.str, s2.len);
    return str;
}

bool
string_cmp(String s1, String s2) {
    return s1.len == s2.len && (memcmp(s1.str, s2.str, s1.len) == 0); 
}

StringNode *
string_list_push(Arena *arena, StringList *list, String str) {
    StringNode **node = &(list->head);
    while (*node) {
        node = &((*node)->next);
    }
    *node = arena_push(arena, sizeof(StringNode));
    (*node)->str = str;
    ++(list->len);
    return *node;
}

StringNode *
string_list_find(StringList *list, String str) {
    StringNode *node = list->head;
    while (node) {
        if (string_cmp(node->str, str)) {
            return node;
        }
        else {
            node = node->next;
        }
    }
    return NULL;
}

#endif // STRING_C
