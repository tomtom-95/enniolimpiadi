#ifndef UI_REGISTRATION_C
#define UI_REGISTRATION_C

#include <unistd.h>

#include "clay.h"
#include "ui_base.c"
#include "raylib/raylib.h"


void
WriteToString(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    ClayVideoDemo_Data *userData = (ClayVideoDemo_Data *)data;

    userData->frame_counter++;

    int MAX_INPUT_CHAR = 31;

    SetMouseCursor(MOUSE_CURSOR_IBEAM);

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
        if (userData->backspace_state == BACKSPACE_NOT_PRESSED) {
            (userData->letterCount)--;
            if (userData->letterCount < 0) {
                userData->letterCount = 0;
            }
            (userData->name)[userData->letterCount] = '\0';

            userData->backspace_state = BACKSPACE_PRESSED; 
        }
        else {
            userData->backspace_count++;
        }

        // TODO(tommaso): overflow?
        if (userData->backspace_count > 50) {
            // Add a small delay to control backspace speed
            static float backspaceTimer = 0.0f;
            backspaceTimer += GetFrameTime();
        
            if ((IsKeyPressed(KEY_BACKSPACE)) || 
                (IsKeyDown(KEY_BACKSPACE) && backspaceTimer >= 0.02f)) {
                backspaceTimer = 0.0f;
                (userData->letterCount)--;
                if (userData->letterCount < 0) {
                    userData->letterCount = 0;
                }
                (userData->name)[userData->letterCount] = '\0';
            }
        }
    }
    else {
        userData->backspace_state = BACKSPACE_NOT_PRESSED;
        userData->backspace_count = 0;
    }

    int key = GetCharPressed();
    while (key > 0) {
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (userData->letterCount < MAX_INPUT_CHAR))
        {
            (userData->name)[userData->letterCount] = (char)key;
            (userData->name)[userData->letterCount+1] = '\0'; // Add null terminator at the end of the string.
            (userData->letterCount)++;
        }
        key = GetCharPressed();  // Check next character in the queue
    }
}

Clay_RenderCommandArray
LayoutRegistrationForm(ClayVideoDemo_Data *data) {
    data->arena_frame->pos = 0;

    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    Clay_Color backgroundColor = {43, 41, 51, 255};
    Clay_Color inputBackgroundColor = {70, 70, 70, 255};
    Clay_Color buttonColor = {120, 90, 210, 255};
    Clay_Color buttonHoverColor = {140, 110, 230, 255};
    Clay_Color textColor = {255, 255, 255, 255};
    Clay_Color placeholderColor = {200, 200, 200, 180};

    // Form container
    CLAY({ 
        .id = CLAY_ID("RegistrationFormContainer"),
        .backgroundColor = backgroundColor,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Form title
        CLAY({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .padding = CLAY_PADDING_ALL(8)
            }
        }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }}}) {}
            CLAY_TEXT(CLAY_STRING("Create an Account"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 28,
                .textColor = textColor
            }));
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }}}) {}
        }

        // Username field
        CLAY({
            .id = CLAY_ID("UsernameContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 8
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Name"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = textColor
            }));

            CLAY({
                .id = CLAY_ID("UsernameInput"),
                .backgroundColor = inputBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_GROW(40)
                    },
                    .padding = CLAY_PADDING_ALL(12)
                }
            }) {
                if (!Clay_Hovered()) {
                    data->frame_counter = 0;
                }
                Clay_OnHover(WriteToString, (intptr_t)data);
                Clay_String clay_string;
                if (((data->frame_counter/60)%2) == 1) {
                    char *chars = arena_push(data->arena_frame, 64); 
                    memcpy(chars, data->name, data->letterCount);
                    chars[data->letterCount] = '_';
                    chars[data->letterCount+1] = '\0';
                    clay_string = (Clay_String) {
                        .isStaticallyAllocated = true,
                        .length = data->letterCount+1,
                        .chars = chars
                    };
                }
                else {
                    clay_string = (Clay_String) {
                        .isStaticallyAllocated = true,
                        .length = data->letterCount,
                        .chars = data->name
                    };
                }
                CLAY_TEXT(clay_string, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = placeholderColor
                }));
            }
        }

        // Register button with hover effect
        CLAY({
            .id = CLAY_ID("RegisterButton"),
            .backgroundColor = Clay_Hovered() ? buttonHoverColor : buttonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_FIXED(56)
                },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_CENTER
                }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Add player"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = textColor,
            }));
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
    }
    return renderCommands;
}

#endif // UI_REGISTRATION_C
