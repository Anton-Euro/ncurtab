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
    int panel_height = max_y - 4;

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
        if (strcmp(path, "/") == 0) {
            snprintf(new_path, MAX_PATH, "/%s", list->entries[cursor].name);
        } else {
            snprintf(new_path, MAX_PATH, "%s/%s", path, list->entries[cursor].name);
        }
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

void delete_file_or_dir(TabManager* manager) {
    Tab* tab = &manager->tabs[manager->active_tab];
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    char* path = tab->active_panel ? tab->left_path : tab->right_path;
    int cursor = tab->active_panel ? tab->left_cursor : tab->right_cursor;

    if (cursor >= list->count) return;

    char full_path[MAX_PATH];
    snprintf(full_path, MAX_PATH, "%s/%s", path, list->entries[cursor].name);

    // Confirmation dialog
    clear();
    mvprintw(0, 0, "Delete %s? (y/n)", list->entries[cursor].name);
    refresh();
    int ch = getch();
    if (ch != 'y' && ch != 'Y') return;

    if (remove_file_or_dir(full_path)) {
        free_file_list(list);
        if (tab->active_panel) {
            tab->left_list = get_directory_contents(path);
            if (tab->left_cursor >= tab->left_list->count) tab->left_cursor = tab->left_list->count - 1;
        } else {
            tab->right_list = get_directory_contents(path);
            if (tab->right_cursor >= tab->right_list->count) tab->right_cursor = tab->right_list->count - 1;
        }
    }
}

void rename_file_or_dir_ui(TabManager* manager) {
    Tab* tab = &manager->tabs[manager->active_tab];
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    char* path = tab->active_panel ? tab->left_path : tab->right_path;
    int cursor = tab->active_panel ? tab->left_cursor : tab->right_cursor;

    if (cursor >= list->count) return;

    char old_path[MAX_PATH];
    snprintf(old_path, MAX_PATH, "%s/%s", path, list->entries[cursor].name);

    // Input new name
    echo();
    curs_set(1);
    clear();
    mvprintw(0, 0, "Enter new name for %s: ", list->entries[cursor].name);
    char new_name[MAX_NAME];
    getstr(new_name);
    noecho();
    curs_set(0);

    if (strlen(new_name) == 0) return;

    char new_path[MAX_PATH];
    snprintf(new_path, MAX_PATH, "%s/%s", path, new_name);

    if (rename_file_or_dir(old_path, new_path)) {
        free_file_list(list);
        if (tab->active_panel) {
            tab->left_list = get_directory_contents(path);
            tab->left_cursor = 0;
        } else {
            tab->right_list = get_directory_contents(path);
            tab->right_cursor = 0;
        }
    }
}

void copy_file_to_other_panel(TabManager* manager) {
    Tab* tab = &manager->tabs[manager->active_tab];
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    char* src_path = tab->active_panel ? tab->left_path : tab->right_path;
    char* dest_path = tab->active_panel ? tab->right_path : tab->left_path;
    int cursor = tab->active_panel ? tab->left_cursor : tab->right_cursor;

    if (cursor >= list->count) return;

    char src_full_path[MAX_PATH];
    char dest_full_path[MAX_PATH];
    snprintf(src_full_path, MAX_PATH, "%s/%s", src_path, list->entries[cursor].name);
    snprintf(dest_full_path, MAX_PATH, "%s/%s", dest_path, list->entries[cursor].name);

    if (copy_file_or_dir(src_full_path, dest_full_path)) {
        free_file_list(tab->active_panel ? tab->right_list : tab->left_list);
        if (tab->active_panel) {
            tab->right_list = get_directory_contents(tab->right_path);
        } else {
            tab->left_list = get_directory_contents(tab->left_path);
        }
    }
}

void move_file_to_other_panel(TabManager* manager) {
    Tab* tab = &manager->tabs[manager->active_tab];
    FileList* list = tab->active_panel ? tab->left_list : tab->right_list;
    char* src_path = tab->active_panel ? tab->left_path : tab->right_path;
    char* dest_path = tab->active_panel ? tab->right_path : tab->left_path;
    int cursor = tab->active_panel ? tab->left_cursor : tab->right_cursor;

    if (cursor >= list->count) return;

    char src_full_path[MAX_PATH];
    char dest_full_path[MAX_PATH];
    snprintf(src_full_path, MAX_PATH, "%s/%s", src_path, list->entries[cursor].name);
    snprintf(dest_full_path, MAX_PATH, "%s/%s", dest_path, list->entries[cursor].name);

    if (move_file_or_dir(src_full_path, dest_full_path)) {
        free_file_list(tab->active_panel ? tab->right_list : tab->left_list);
        free_file_list(list);
        if (tab->active_panel) {
            tab->right_list = get_directory_contents(tab->right_path);
            tab->left_list = get_directory_contents(tab->left_path);
            if (tab->left_cursor >= tab->left_list->count) tab->left_cursor = tab->left_list->count - 1;
        } else {
            tab->left_list = get_directory_contents(tab->left_path);
            tab->right_list = get_directory_contents(tab->right_path);
            if (tab->right_cursor >= tab->right_list->count) tab->right_cursor = tab->right_list->count - 1;
        }
    }
}

void free_tabs(TabManager* manager) {
    for (int i = 0; i < manager->tab_count; i++) {
        free_file_list(manager->tabs[i].left_list);
        free_file_list(manager->tabs[i].right_list);
    }
    free(manager);
}