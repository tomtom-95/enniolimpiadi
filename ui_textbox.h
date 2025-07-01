#ifndef UI_TEXTBOX_H
#define UI_TEXTBOX_H

#include "ui_utils.h"

void UpdateCursorPos(TextBoxData *textBoxData);
void UpdateHighlightPos(TextBoxData *textBoxData);

void TextBoxWrite(TextBoxData *textBoxData, int key);
void TextBoxDelete(TextBoxData *textBoxData);
void TextBoxPaste(TextBoxData *textBoxData, u8 *clipboardText);
u8 *TextBoxCopy(TextBoxData *textBoxData);

void TextBoxMoveCursorLeft(TextBoxData *textBoxData);
void TextBoxMoveCursorRight(TextBoxData *textBoxData);

void HelperTextBox(TextBoxData *textBoxData, Clay_String textBoxIdStr);

#endif // UI_TEXTBOX_H
