#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle) \
    (Rectangle) {                                     \
        .x = rectangle.x,                             \
        .y = rectangle.y,                             \
        .width = rectangle.width,                     \
        .height = rectangle.height                    \
    }

#define CLAY_COLOR_TO_RAYLIB_COLOR(color)    \
    (Color) {                                \
        .r = (unsigned char)roundf(color.r), \
        .g = (unsigned char)roundf(color.g), \
        .b = (unsigned char)roundf(color.b), \
        .a = (unsigned char)roundf(color.a)  \
    }

#define MegaByte(x) (((uint64_t)(1024ULL*1024ULL))*x)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

typedef struct Arena Arena;
struct Arena {
    u8 *data;
    u64 pos;
    u64 size;
};

typedef struct {
    float yOffset;
    Arena *arena_frame;
    Arena *arena_permanent;
} LayoutData;


Clay_Color white = {255, 255, 255, 255};

// Blueish
Clay_Color blue = {33, 31, 41, 255};
Clay_Color blue_ligth = {63, 61, 71, 255};

// Gray
Clay_Color gray = {90, 90, 90, 255};
Clay_Color gray_light = {140, 140, 140, 255};
Clay_Color gray_lighter = {160, 160, 160, 255};

// Violet
Clay_Color violet = {120, 90, 210, 255};
Clay_Color violet_light = {140, 110, 230, 255};

Clay_Sizing layoutExpand = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };
Font fonts[1];

Arena
arena_alloc(u64 size) {
    Arena arena = {0};

    u8 *data = malloc(size);
    assert(data);

    arena.pos  = 0;
    arena.data = data;
    arena.size = size;

    return arena;
}

void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags) {
    SetConfigFlags(flags);
    InitWindow(width, height, title);
}

// A MALLOC'd buffer, that we keep modifying inorder to save from so many Malloc and Free Calls.
// Call Clay_Raylib_Close() to free
static char *temp_render_buffer = NULL;
static int temp_render_buffer_len = 0;

// Call after closing the window to clean up the render buffer
void
Clay_Raylib_Close(void) {
    if(temp_render_buffer) {
        free(temp_render_buffer);
    }
    temp_render_buffer_len = 0;

    CloseWindow();
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    Clay_Raylib_Initialize(
        1024, 768, "Enniolimpiadi",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    );

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
        clayRequiredMemory, malloc(clayRequiredMemory)
    );

    Clay_Initialize(
        clayMemory, (Clay_Dimensions) {.width = GetScreenWidth(), .height = GetScreenHeight()},
        (Clay_ErrorHandler) { HandleClayErrors }
    );

    Clay_SetDebugModeEnabled(false);

    fonts[0] = LoadFontEx("resources/FiraCode-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);

    Arena arena_frame = arena_alloc(MegaByte(1));

    LayoutData layoutData = {
        .arena_frame = &arena_frame,
    };

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) { .width = GetScreenWidth(), .height = GetScreenHeight() });

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));
        Clay_UpdateScrollContainers(false, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());

        Clay_ElementId textBoxId = Clay_GetElementId(CLAY_STRING("TextBox"));
        if (Clay_PointerOver(textBoxId)) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }

        layoutData.arena_frame->pos = 0;

        Clay_BeginLayout();
        CLAY({ .id = CLAY_ID("MainWindow"),
            .backgroundColor = blue,
            .layout = {
                .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
                .padding = CLAY_PADDING_ALL(16)
            }
        }) {
            CLAY({
                .id = CLAY_ID("MainContent"),
                .backgroundColor = gray,
                .layout = {
                    .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
                    .padding = CLAY_PADDING_ALL(16)
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                Clay_Color colorBorder = {255, 255, 255, 255};
                u16 widthBorder = 3;

                CLAY({
                    .id = CLAY_ID("TextBox"),
                    .backgroundColor = blue,
                    .cornerRadius = CLAY_CORNER_RADIUS(6),
                    .border = {
                        .color = colorBorder,
                        .width = { widthBorder, widthBorder, widthBorder, widthBorder }
                    },
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(40) },
                        .padding = CLAY_PADDING_ALL(12)
                    }
                }) {}
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();
        for (int32_t i = 0; i < renderCommands.length; i++) {
            Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += layoutData.yOffset;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int j = 0; j < renderCommands.length; j++) {
            Clay_RenderCommand *renderCommand = (
                Clay_RenderCommandArray_Get(&renderCommands, j)
            );
            Clay_BoundingBox boundingBox = renderCommand->boundingBox;
            switch (renderCommand->commandType) {
                case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                    Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                    if (config->cornerRadius.topLeft > 0) {
                        float radius;
                        if (boundingBox.width > boundingBox.height) {
                            radius = (config->cornerRadius.topLeft * 2) / (float)boundingBox.height;
                        }
                        else {
                            radius = (config->cornerRadius.topLeft * 2) / (float)boundingBox.width;
                        }
                        Rectangle raylib_rectangle = {
                            .x = boundingBox.x, .y = boundingBox.y,
                            .width = boundingBox.width, .height = boundingBox.height,
                        };
                        DrawRectangleRounded(raylib_rectangle, radius, 8, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                    }
                    else {
                        DrawRectangle(
                            (int)boundingBox.x, (int)boundingBox.y, (int)boundingBox.width, (int)boundingBox.height,
                            CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor)
                        );
                    }
                    break;
                }
            }
        }
        EndDrawing();
    }

    Clay_Raylib_Close();

    return 0;
}
