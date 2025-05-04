#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include <string.h>
#include "filesystem.h"
#include "types.h"


void init_ui_windows(UIWindows* ui);
void resize_ui_windows(UIWindows* ui);
void draw_ui(UIWindows* ui, TabManager* tab_manager);
void draw_help_window(UIWindows* ui);
bool prompt_new_name(UIWindows* ui, const char* old_name, char* new_name);
bool confirm_delete(UIWindows* ui, const char* name);
void free_ui_windows(UIWindows* ui);

#endif