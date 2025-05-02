#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <ncurses.h>
#include "tabs.h"
#include "filesystem.h"

TabManager* init_tabs() {
    TabManager* manager = malloc(sizeof(TabManager));
    manager->tab_count = 0;
    manager->active_tab = 0;
    return manager;
}

void add_tab(TabManager* manager, const char* path) {
    if (manager->tab_count >= MAX_TABS) return;

    Tab* tab = &manager->tabs[manager->tab_count];
    strncpy(tab->left_path, path, MAX_PATH - 1);
    strncpy(tab->right_path, path, MAX_PATH - 1);
    tab->left_path[MAX_PATH - 1] = '\0';
    tab->right_path[MAX_PATH - 1] = '\0';
    tab->left_list = get_directory_contents(tab->left_path);
    tab->right_list = get_directory_contents(tab->right_path);
    tab->left_cursor = 0;
    tab->right_cursor = 0;
    tab->left_offset = 0;
    tab->right_offset = 0;
    tab->active_panel = true;

    manager->tab_count++;
    manager->active_tab = manager->tab_count - 1;
}

void switch_tab(TabManager* manager, int index) {
    if (index >= 0 && index < manager->tab_count) {
        manager->active_tab = index;
    }
}

void move_cursor(Tab* tab, int direction) {
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    int* cursor = tab->active_panel ? &tab->left_cursor : &tab->right_cursor;
    int* offset = tab->active_panel ? &tab->left_offset : &tab->right_offset;

    *cursor += direction;
    if (*cursor < 0) {
        *cursor = 0;
        *offset = 0;
    }
    if (*cursor >= list->count) {
        *cursor = list->count - 1;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int panel_height = max_y - 2;

    if (*cursor > *offset + panel_height - 3) {
        *offset = *cursor - (panel_height - 3);
        if (*offset > list->count - panel_height) {
            *offset = list->count - panel_height;
        }
    }
    if (*cursor < *offset + 3) {
        *offset = *cursor - 3;
        if (*offset < 0) {
            *offset = 0;
        }
    }
}

void enter_directory(TabManager* manager) {
    Tab* tab = &manager->tabs[manager->active_tab];
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    char* path = tab->active_panel ? tab->left_path : tab->right_path;
    int cursor = tab->active_panel ? tab->left_cursor : tab->right_cursor;

    if (cursor >= list->count || !list->entries[cursor].is_dir) return;

    char new_path[MAX_PATH];
    if (strcmp(list->entries[cursor].name, "..") == 0) {
        char temp_path[MAX_PATH];
        strncpy(temp_path, path, MAX_PATH);
        char* parent = dirname(temp_path);
        strncpy(new_path, parent, MAX_PATH - 1);
        new_path[MAX_PATH - 1] = '\0';
    } else {
        snprintf(new_path, MAX_PATH, "%s/%s", path, list->entries[cursor].name);
    }

    free_file_list(list);
    if (tab->active_panel) {
        tab->left_list = get_directory_contents(new_path);
        tab->left_cursor = 0;
        tab->left_offset = 0;
        strncpy(tab->left_path, new_path, MAX_PATH);
    } else {
        tab->right_list = get_directory_contents(new_path);
        tab->right_cursor = 0;
        tab->right_offset = 0;
        strncpy(tab->right_path, new_path, MAX_PATH);
    }
}

void free_tabs(TabManager* manager) {
    for (int i = 0; i < manager->tab_count; i++) {
        free_file_list(manager->tabs[i].left_list);
        free_file_list(manager->tabs[i].right_list);
    }
    free(manager);
}