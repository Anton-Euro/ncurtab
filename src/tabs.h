#ifndef TABS_H
#define TABS_H

#include "filesystem.h"

#define MAX_TABS 10

typedef struct {
    char left_path[MAX_PATH];
    char right_path[MAX_PATH];
    FileList* left_list;
    FileList* right_list;
    int left_cursor;
    int right_cursor;
    int left_offset;
    int right_offset;
    bool active_panel; // true: left, false: right
} Tab;

typedef struct {
    Tab tabs[MAX_TABS];
    int tab_count;
    int active_tab;
} TabManager;

TabManager* init_tabs();
void add_tab(TabManager* manager, const char* path);
void switch_tab(TabManager* manager, int index);
void move_cursor(Tab* tab, int direction);
void enter_directory(TabManager* manager);
void free_tabs(TabManager* manager);
void move_file_to_other_panel(TabManager* manager);
void copy_file_to_other_panel(TabManager* manager);
void rename_file_or_dir_ui(TabManager* manager);
void delete_file_or_dir(TabManager* manager);

#endif