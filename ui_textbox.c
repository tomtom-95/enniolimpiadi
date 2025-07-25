#ifndef UI_TEXTBOX_C
#define UI_TEXTBOX_C

#include "ui_textbox.h"

void
UpdateCursorPos(TextBoxData *textBoxData)
{
    char strCursor[textBoxData->strLenMax];
    memcpy(strCursor, textBoxData->strUser.str, textBoxData->cursorIdx);
    strCursor[textBoxData->cursorIdx] = '\0';
    textBoxData->cursorPos = MeasureTextEx(
        textBoxData->font, strCursor, textBoxData->fontSize, 0
    );
}

void
UpdateHighlightPos(TextBoxData *textBoxData)
{
    char strHighlight[textBoxData->strLenMax];
    memcpy(strHighlight, textBoxData->strUser.str, textBoxData->highlightIdx);
    strHighlight[textBoxData->highlightIdx] = '\0';
    textBoxData->highlightPos = MeasureTextEx(
        textBoxData->font, strHighlight, textBoxData->fontSize, 0
    );
}

void
TextBoxWrite(TextBoxData *textBoxData, int key)
{
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;
    memmove(p_start + 1, p_end, strUser->len - end);

    strUser->str[start] = (u8)key;
    strUser->len = strUser->len + start - end + 1;

    *cursorIdx = start + 1;
    *highlightIdx = textBoxData->cursorIdx;
}

void
TextBoxDelete(TextBoxData *textBoxData)
{
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    if (start == end && start > 0) {
        --start;
    }

    if (start != end) {
        u8 *p_start = strUser->str + start;
        u8 *p_end = strUser->str + end;

        memmove(p_start, p_end, strUser->len - end);
        strUser->len -= (end - start);
    }

    *cursorIdx = start;
    *highlightIdx = start;
}

u8 *
TextBoxCopy(TextBoxData *textBoxData)
{
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *selectedText = arena_push(layoutData.arena_frame, end - start + 1);
    memcpy(selectedText, textBoxData->strUser.str + start, end - start);
    selectedText[end - start] = '\0';

    return selectedText;
}

void
TextBoxPaste(TextBoxData *textBoxData, u8 *clipboardText)
{
    s16 textLen = (s16)(cstring_len(clipboardText) - 1);
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    s16 start = (s16)Min(*cursorIdx, *highlightIdx);
    s16 end = (s16)Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;

    s16 delta = textLen - end + start;
    memmove(p_end + delta, p_end, textBoxData->strUser.len - end);
    memcpy(p_start, clipboardText, textLen);

    textBoxData->strUser.len += delta;
    textBoxData->cursorIdx = end + delta;
    textBoxData->highlightIdx = end + delta;
}


void
TextBoxMoveCursorLeft(TextBoxData *textBoxData)
{
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);
    if (*cursorIdx == *highlightIdx && *cursorIdx > 0) {
        --(*cursorIdx);
        --(*highlightIdx);
    }
    else {
        u16 idx = Min(*cursorIdx, *highlightIdx);
        *cursorIdx = idx;
        *highlightIdx = idx;
    }
}

void
TextBoxMoveCursorRight(TextBoxData *textBoxData)
{
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);
    if (*cursorIdx == *highlightIdx && *cursorIdx < textBoxData->strUser.len) {
        ++(*cursorIdx);
        ++(*highlightIdx);
    }
    else {
        u16 idx = Max(*cursorIdx, *highlightIdx);
        *cursorIdx = idx;
        *highlightIdx = idx;
    }
}

void
HelperTextBox(TextBoxData *textBoxData, Clay_String textBoxIdStr)
{
    Clay_ElementId textBoxId = Clay_GetElementId(textBoxIdStr);

    assert(textBoxData->cursorIdx <= textBoxData->strUser.len);

    if (layoutData.last_element_clicked.id == textBoxId.id) {
        textBoxData->colorBorder = gray_light;
        textBoxData->widthBorder = 3;
        ++textBoxData->frameCounter;
        textBoxData->isKeyPressedThisFrame = false;

        // Handle user writing
        int key = GetCharPressed();
        while (key > 0) {
            textBoxData->frameCounter = 0;
            if ((key >= 32) && (key <= 125) && (textBoxData->strUser.len < textBoxData->strLenMax)) {
                textBoxData->isKeyPressedThisFrame = true;
                TextBoxWrite(textBoxData, key);
                key = GetCharPressed();
            }
        }

        bool isCtrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        bool isCmdDown  = IsKeyDown(KEY_LEFT_SUPER)   || IsKeyDown(KEY_RIGHT_SUPER);

        bool isCopyCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_C);
        bool isPasteCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_V);

        if (isCopyCombo) {
            textBoxData->isKeyPressedThisFrame = true;
            u8* selectedText = TextBoxCopy(textBoxData);
            SetClipboardText(selectedText);
        }

        if (isPasteCombo) {
            textBoxData->isKeyPressedThisFrame = true;
            const char *clipboardText = GetClipboardText();
            if (clipboardText) {
                // TODO: check implementation
                TextBoxPaste(textBoxData, clipboardText);
            }
        }

        // Determine if any control key is being held
        if (IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            int currentKey = 0;
            if (IsKeyDown(KEY_BACKSPACE))  currentKey = KEY_BACKSPACE;
            else if (IsKeyDown(KEY_LEFT))  currentKey = KEY_LEFT;
            else if (IsKeyDown(KEY_RIGHT)) currentKey = KEY_RIGHT;

            if (!textBoxData->isKeyHeld || textBoxData->keyHeld != currentKey) {
                // New key press or key changed
                textBoxData->keyHeld = currentKey;
                textBoxData->keyRepeatTimer = textBoxData->keyRepeatDelay;
                textBoxData->isKeyHeld = true;

                // Immediate action
                if (currentKey == KEY_BACKSPACE) {
                    TextBoxDelete(textBoxData);
                }
                else if (currentKey == KEY_LEFT) {
                    TextBoxMoveCursorLeft(textBoxData);
                }
                else if (currentKey == KEY_RIGHT) {
                    TextBoxMoveCursorRight(textBoxData);
                }
            }
            else {
                // Handle repeat
                textBoxData->keyRepeatTimer -= GetFrameTime();
                if (textBoxData->keyRepeatTimer <= 0.0f) {
                    if (textBoxData->keyHeld == KEY_BACKSPACE) {
                        TextBoxDelete(textBoxData);
                    }
                    else if (textBoxData->keyHeld == KEY_LEFT) {
                        TextBoxMoveCursorLeft(textBoxData);
                    }
                    else if (textBoxData->keyHeld == KEY_RIGHT) {
                        TextBoxMoveCursorRight(textBoxData);
                    }
                    textBoxData->keyRepeatTimer = textBoxData->keyRepeatRate;
                }
            }

            textBoxData->isKeyPressedThisFrame = true;
            textBoxData->frameCounter = 0;
        }
        else {
            textBoxData->isKeyHeld = false;
            textBoxData->keyHeld = 0;
            textBoxData->keyRepeatTimer = 0.0f;
        }

        UpdateCursorPos(textBoxData);
        UpdateHighlightPos(textBoxData);

        string_cpy(&textBoxData->strOutput, &textBoxData->strUser);
    }
    else {
        textBoxData->colorBorder = blue;
        textBoxData->widthBorder = 1;
        textBoxData->frameCounter = textBoxData->cursorFrequency;

        if (textBoxData->strUser.len == 0) {
            string_cpy(&textBoxData->strOutput, &textBoxData->strLabel);
        }
    }
}

#endif // UI_TEXTBOX_C
