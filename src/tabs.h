#ifndef TABS_H
#define TABS_H

#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <ncurses.h>
#include "filesystem.h"
#include "types.h"

TabManager* init_tabs();
void add_tab(TabManager* manager, const char* path);
void switch_tab(TabManager* manager, int index);
void move_cursor(Tab* tab, int direction);
void enter_directory(TabManager* manager);
void free_tabs(TabManager* manager);
void move_entity_to_other_panel(TabManager* manager);
void copy_entity_to_other_panel(TabManager* manager);
void rename_entity(TabManager* manager, UIWindows* ui);
void delete_entity(TabManager* manager, UIWindows* ui);

#endif