/*******************************************************************************************
*
*   raylib [text] example - Input Box
*
*   Example complexity rating: [★★☆☆] 2/4
*
*   Example originally created with raylib 1.7, last time updated with raylib 3.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2017-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib/raylib.h"

#define MAX_INPUT_CHARS 16

void
DrawTrimmedRectangle(Rectangle textBox, char *label, Color color) {
    Vector2 p0 = {.x = textBox.x, .y = textBox.y};
    Vector2 p1 = {.x = textBox.x + 110, .y = textBox.y};
    Vector2 p2 = {.x = textBox.x + textBox.width, .y = textBox.y};
    Vector2 p3 = {.x = textBox.x + textBox.width, .y = textBox.y + textBox.height};
    Vector2 p4 = {.x = textBox.x, .y = textBox.y + textBox.height};

    DrawLine(p1.x, p1.y, p2.x, p2.y, color);
    DrawLine(p2.x, p2.y, p3.x, p3.y, color);
    DrawLine(p3.x, p3.y, p4.x, p4.y, color);
    DrawLine(p4.x, p4.y, p0.x, p0.y, color);
    DrawText(label, p0.x + 5, p0.y - 5, 10, MAROON);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [text] example - input box");

    char label[] = "Insert player name";
    char name[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBox = { screenWidth/2.0f - 200, 180, 400, 50 };
    bool mouseOnText = false;
    bool clickOnText = false;

    int framesCounter = 0;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (CheckCollisionPointRec(GetMousePosition(), textBox)) {
            mouseOnText = true;
        }
        else {
            mouseOnText = false;
        }

        if (mouseOnText) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                clickOnText = true;
            }
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            ++framesCounter;
        }
        else {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                clickOnText = false;
            }
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            framesCounter = 0;
        }

        if (clickOnText) {
            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0) {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                {
                    name[letterCount] = (char)key;
                    name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                    letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                name[letterCount] = '\0';
            }
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("PLACE MOUSE OVER INPUT BOX!", 240, 140, 20, GRAY);

            DrawRectangleRec(textBox, RAYWHITE);
            if (clickOnText) {
                DrawTrimmedRectangle(textBox, label, RED);
                DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
            }
            else {
                if (mouseOnText) {
                    if (letterCount == 0) {
                        DrawRectangleLines(
                            (int)textBox.x,
                            (int)textBox.y,
                            (int)textBox.width,
                            (int)textBox.height, RED
                        );
                        DrawText(label, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
                    }
                    else {
                        DrawTrimmedRectangle(textBox, label, RED);
                        DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
                    }
                }
                else {
                    if (letterCount == 0) {
                        DrawRectangleLines(
                            (int)textBox.x,
                            (int)textBox.y,
                            (int)textBox.width,
                            (int)textBox.height, DARKGRAY
                        );
                        DrawText(label, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
                    }
                    else {
                        DrawTrimmedRectangle(textBox, label, BLACK);
                        DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
                    }
                }
            }

            DrawText(
                TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_INPUT_CHARS),
                315, 250, 20, DARKGRAY
            );

            if (clickOnText) {
                if (letterCount < MAX_INPUT_CHARS) {
                    // Draw blinking underscore char
                    if (((framesCounter/20)%2) == 0) {
                        DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40), (int)textBox.y + 12, 40, MAROON);
                    }
                }
                else {
                    DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
                }
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Check if any key is pressed
// NOTE: We limit keys check to keys between 32 (KEY_SPACE) and 126
bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}
