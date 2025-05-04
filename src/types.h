#ifndef TYPES_H
#define TYPES_H

#include <ncurses.h>

#define MAX_TABS 10
#define MAX_PATH 256
#define MAX_FILES 256
#define MAX_NAME 64

typedef struct {
    char name[MAX_NAME];
    bool is_dir;
} FileEntry;

typedef struct {
    FileEntry entries[MAX_FILES];
    int count;
} FileList;

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

typedef struct {
    WINDOW* tabs_win;
    WINDOW* panels_container_win;
    WINDOW* left_panel_win;
    WINDOW* right_panel_win;
    WINDOW* status_win;
    int max_y;
    int max_x;
} UIWindows;

#endif