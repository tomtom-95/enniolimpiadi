#ifndef DRAWING_C
#define DRAWING_C

#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#include "utils.c"

String
CreateLabelStr(Arena *arena, u32 round, u32 idx)
{
    String str_round = { .str = arena_push(arena, 11) };
    str_round.len = snprintf(str_round.str, 11, "%u", round);

    String str_num = { .str = arena_push(arena, 11) };
    str_num.len = snprintf(str_num.str, 11, "%u", idx);

    String s0 = string_cat(arena, string_from_cstring_lit("label"), string_from_cstring_lit("_"));
    String s1 = string_cat(arena, s0, str_round);
    String s2 = string_cat(arena, s1, string_from_cstring_lit("_"));
    String result = string_cat(arena, s2, str_num);

    return result;
}

u32
GetRoundsNum(u32 numPlayers)
{
    u32 power_of_two = flp2(numPlayers);

    u32 v = power_of_two; // 32-bit word input to count zero bits on right
    u32 c = 32;           // c will be the number of zero bits on the right

    v &= -(signed)(v);
    if (v) c--;
    if (v & 0x0000FFFF) c -= 16;
    if (v & 0x00FF00FF) c -= 8;
    if (v & 0x0F0F0F0F) c -= 4;
    if (v & 0x33333333) c -= 2;
    if (v & 0x55555555) c -= 1;

    return c + 1;
}

void
BezierWrapper(u32 round, u32 num)
{
    u32 next;
    if (num % 2 == 0) {
        next = num / 2;
    } else {
        next = num / 2 + 1;
    }

    String s0 = CreateLabelStr(layoutData.arena_frame, round, num);
    String s1 = CreateLabelStr(layoutData.arena_frame, round + 1, next);

    Clay_ElementData box0 = (
        Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(s0)))
    );
    float x0 = box0.boundingBox.x + box0.boundingBox.width;
    float y0 = box0.boundingBox.y;
    Vector2 startPoint = { x0, y0 + box0.boundingBox.height / 2 };

    Clay_ElementData box1 = (
        Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(s1)))
    );
    float x1 = box1.boundingBox.x;
    float y1 = box1.boundingBox.y;
    Vector2 endPoint = { x1, y1 + box1.boundingBox.height / 2 };

    DrawLineBezier(startPoint, endPoint, 1.0f, BLACK);
}

void
DrawBezierCurves(u32 numPlayers) {
    u32 roundsNum = GetRoundsNum(numPlayers);

    u32 power_of_two = flp2(numPlayers);
    u32 remainder = (numPlayers - power_of_two) * 2;

    while (remainder) {
        BezierWrapper(0, remainder);
        --remainder;
    }
    for (int i = 1; i < roundsNum; ++i) {
        for (int j = 1; j <= power_of_two; ++j) {
            BezierWrapper(i, j);
        }
        power_of_two = power_of_two / 2;
    }
}

#endif
