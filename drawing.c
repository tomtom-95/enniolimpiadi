#ifndef DRAWING_C
#define DRAWING_C

#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#include "utils.c"

void
DrawBezierCurves(u32 numPlayers) {
    // I can start from number of players in the currently selected tournament
    // to get what kind of label
    // I have recorded
    // for example: if I have 16 players
    //      -> label_0_1 -> label_0_16
    //      -> label_1_1 -> label_1_8
    //      -> label_2_1 -> label_2_4
    //      -> label_3_1 -> label_3_2
    //      -> label_4_1

    u32 power_of_two = flp2(numPlayers);
    u32 remainder = (numPlayers - power_of_two) * 2;
    u32 tmp = 2 * power_of_two;

    // TODO: some bit magic that I must study
    u32 v = power_of_two; // 32-bit word input to count zero bits on right
    u32 c = 32;           // c will be the number of zero bits on the right

    v &= -(signed)(v);
    if (v) c--;
    if (v & 0x0000FFFF) c -= 16;
    if (v & 0x00FF00FF) c -= 8;
    if (v & 0x0F0F0F0F) c -= 4;
    if (v & 0x33333333) c -= 2;
    if (v & 0x55555555) c -= 1;

    u32 numRounds;
    if (numPlayers == power_of_two) {
        numRounds = c;
    }
    else {
        numRounds = c + 1;
    }

    if (numPlayers == power_of_two) {
        for (int i = 1; i <= numRounds; ++i) {
            for (int j = 1; j <= power_of_two; ++j) {
                String str_label = string_from_cstring_lit("label");
                String str_underscore = string_from_cstring_lit("_");

                String *str_round = arena_push(layoutData.arena_frame, sizeof(String));
                str_round->str = arena_push(layoutData.arena_frame, 11);
                str_round->len = snprintf(str_round->str, 11, "%u", i);

                String *str_num = arena_push(layoutData.arena_frame, sizeof(String));
                str_num->str = arena_push(layoutData.arena_frame, 11);
                str_num->len = snprintf(str_num->str, 11, "%u", j);

                String tmp0   = string_cat(layoutData.arena_frame, str_label, str_underscore);
                String tmp1   = string_cat(layoutData.arena_frame, tmp0, *str_round);
                String tmp2   = string_cat(layoutData.arena_frame, tmp1, str_underscore);
                String final0 = string_cat(layoutData.arena_frame, tmp2, *str_num);

                String *str_round_next = arena_push(layoutData.arena_frame, sizeof(String));
                str_round_next->str = arena_push(layoutData.arena_frame, 11);
                str_round_next->len = snprintf(str_round_next->str, 11, "%u", i + 1);

                String *str_num_next = arena_push(layoutData.arena_frame, sizeof(String));
                str_num_next->str = arena_push(layoutData.arena_frame, 11);
                u32 next;
                if (j % 2 == 0) {
                    next = j / 2;
                } else {
                    next = j / 2 + 1;
                }
                str_num_next->len = snprintf(str_num_next->str, 11, "%u", next);

                String tmp0_next   = string_cat(layoutData.arena_frame, str_label, str_underscore);
                String tmp1_next   = string_cat(layoutData.arena_frame, tmp0_next, *str_round_next);
                String tmp2_next   = string_cat(layoutData.arena_frame, tmp1_next, str_underscore);
                String final0_next = string_cat(layoutData.arena_frame, tmp2_next, *str_num_next);

                Clay_ElementData label_0 = (
                    Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0)))
                );
                float x0 = label_0.boundingBox.x + label_0.boundingBox.width;
                float y0 = label_0.boundingBox.y;

                Clay_ElementData label_1 = (
                    Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0_next)))
                );
                float x1 = label_1.boundingBox.x;
                float y1 = label_1.boundingBox.y;

                Vector2 startPoint = { x0, y0 + label_0.boundingBox.height / 2};
                Vector2 endPoint = { x1, y1 + label_1.boundingBox.height / 2};

                DrawLineBezier(startPoint, endPoint, 1.0f, BLACK);
            }
            power_of_two = power_of_two / 2;
        }
    }
    else {
        while (remainder) {
            String str_label = string_from_cstring_lit("label");
            String str_underscore = string_from_cstring_lit("_");

            String *str_round = arena_push(layoutData.arena_frame, sizeof(String));
            str_round->str = arena_push(layoutData.arena_frame, 11);
            str_round->len = snprintf(str_round->str, 11, "%u", 0);

            String *str_num = arena_push(layoutData.arena_frame, sizeof(String));
            str_num->str = arena_push(layoutData.arena_frame, 11);
            str_num->len = snprintf(str_num->str, 11, "%u", remainder);

            String tmp0   = string_cat(layoutData.arena_frame, str_label, str_underscore);
            String tmp1   = string_cat(layoutData.arena_frame, tmp0, *str_round);
            String tmp2   = string_cat(layoutData.arena_frame, tmp1, str_underscore);
            String final0 = string_cat(layoutData.arena_frame, tmp2, *str_num);

            String *str_round_next = arena_push(layoutData.arena_frame, sizeof(String));
            str_round_next->str = arena_push(layoutData.arena_frame, 11);
            str_round_next->len = snprintf(str_round_next->str, 11, "%u", 1);

            String *str_num_next = arena_push(layoutData.arena_frame, sizeof(String));
            str_num_next->str = arena_push(layoutData.arena_frame, 11);
            u32 next;
            if (remainder % 2 == 0) {
                next = remainder / 2;
            } else {
                next = remainder / 2 + 1;
            }
            str_num_next->len = snprintf(str_num_next->str, 11, "%u", next);

            String tmp0_next   = string_cat(layoutData.arena_frame, str_label, str_underscore);
            String tmp1_next   = string_cat(layoutData.arena_frame, tmp0_next, *str_round_next);
            String tmp2_next   = string_cat(layoutData.arena_frame, tmp1_next, str_underscore);
            String final0_next = string_cat(layoutData.arena_frame, tmp2_next, *str_num_next);

            Clay_ElementData label_0 = (
                Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0)))
            );
            float x0 = label_0.boundingBox.x + label_0.boundingBox.width;
            float y0 = label_0.boundingBox.y;

            Clay_ElementData label_1 = (
                Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0_next)))
            );
            float x1 = label_1.boundingBox.x;
            float y1 = label_1.boundingBox.y;

            Vector2 startPoint = { x0, y0 + label_0.boundingBox.height / 2};
            Vector2 endPoint = { x1, y1 + label_1.boundingBox.height / 2};

            DrawLineBezier(startPoint, endPoint, 1.0f, BLACK);

            --remainder;
        }
        for (int i = 1; i < numRounds; ++i) {
            for (int j = 1; j <= power_of_two; ++j) {
                String str_label = string_from_cstring_lit("label");
                String str_underscore = string_from_cstring_lit("_");

                String *str_round = arena_push(layoutData.arena_frame, sizeof(String));
                str_round->str = arena_push(layoutData.arena_frame, 11);
                str_round->len = snprintf(str_round->str, 11, "%u", i);

                String *str_num = arena_push(layoutData.arena_frame, sizeof(String));
                str_num->str = arena_push(layoutData.arena_frame, 11);
                str_num->len = snprintf(str_num->str, 11, "%u", j);

                String tmp0   = string_cat(layoutData.arena_frame, str_label, str_underscore);
                String tmp1   = string_cat(layoutData.arena_frame, tmp0, *str_round);
                String tmp2   = string_cat(layoutData.arena_frame, tmp1, str_underscore);
                String final0 = string_cat(layoutData.arena_frame, tmp2, *str_num);

                String *str_round_next = arena_push(layoutData.arena_frame, sizeof(String));
                str_round_next->str = arena_push(layoutData.arena_frame, 11);
                str_round_next->len = snprintf(str_round_next->str, 11, "%u", i + 1);

                String *str_num_next = arena_push(layoutData.arena_frame, sizeof(String));
                str_num_next->str = arena_push(layoutData.arena_frame, 11);
                u32 next;
                if (j % 2 == 0) {
                    next = j / 2;
                } else {
                    next = j / 2 + 1;
                }
                str_num_next->len = snprintf(str_num_next->str, 11, "%u", next);

                String tmp0_next   = string_cat(layoutData.arena_frame, str_label, str_underscore);
                String tmp1_next   = string_cat(layoutData.arena_frame, tmp0_next, *str_round_next);
                String tmp2_next   = string_cat(layoutData.arena_frame, tmp1_next, str_underscore);
                String final0_next = string_cat(layoutData.arena_frame, tmp2_next, *str_num_next);

                Clay_ElementData label_0 = (
                    Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0)))
                );
                float x0 = label_0.boundingBox.x + label_0.boundingBox.width;
                float y0 = label_0.boundingBox.y;

                Clay_ElementData label_1 = (
                    Clay_GetElementData(Clay_GetElementId(Clay_String_from_String(final0_next)))
                );
                float x1 = label_1.boundingBox.x;
                float y1 = label_1.boundingBox.y;

                Vector2 startPoint = { x0, y0 + label_0.boundingBox.height / 2};
                Vector2 endPoint = { x1, y1 + label_1.boundingBox.height / 2};

                DrawLineBezier(startPoint, endPoint, 1.0f, BLACK);
            }
            power_of_two = power_of_two / 2;
        }
    }
}

#endif
