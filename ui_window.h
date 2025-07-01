#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <stdlib.h>

#include "utils.c"
#include "arena.c"
#include "string.c"
#include "registration.c"

#include "clay.h"
#include "raylib/raylib.h"
#include "ui_textbox.c"
#include "ui_interaction.c"
#include "ui_header.c"

void
HandleClayErrors(Clay_ErrorData errorData);

Clay_RenderCommandArray
GetLayout(void);

void
LayoutFloatingMenu_(String string, RegistrationMap *primary_map, RegistrationMap *link_map);

void
LayoutPlayerFloatingMenu(String player_string, RegistrationMap *player_map, RegistrationMap *tournament_map);

void
LayoutTournamentFloatingMenu(String tournament_string,
    RegistrationMap *player_map, RegistrationMap *tournament_map);

void
LayoutAddPlayerWindow(LayoutData *layoutData);

void
LayoutAddTournamentWindow(LayoutData *layoutData);

void
LayoutPlayersWindow(void);

void
LayoutTournamentsWindow(void);

#endif // UI_WINDOW_H
