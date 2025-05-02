#include <ncurses.h>
#include <string.h>
#include "ui.h"
#include "filesystem.h"

void draw_panel(FileList* list, int cursor, int offset, int start_y, int start_x, int height, int width, bool active) {
    for (int i = 0; i < height && (i + offset) < list->count; i++) {
        int idx = i + offset;
        bool is_selected = (idx == cursor && active);

        if (list->entries[idx].is_dir) {
            attron(COLOR_PAIR(3));
        } else {
            attron(COLOR_PAIR(4));
        }

        if (is_selected) {
            attron(A_REVERSE);
        }

        mvprintw(start_y + i, start_x, "%-*s", width - 1, list->entries[idx].name);

        if (is_selected) {
            attroff(A_REVERSE);
        }
        if (list->entries[idx].is_dir) {
            attroff(COLOR_PAIR(3));
        } else {
            attroff(COLOR_PAIR(4));
        }
    }
}

void draw_tabs(TabManager* tab_manager, int start_y, int width) {
    for (int i = 0; i < tab_manager->tab_count; i++) {
        if (i == tab_manager->active_tab) {
            attron(COLOR_PAIR(2));
        }
        mvprintw(start_y, i * 20, "Tab %d: %s", i + 1, tab_manager->tabs[i].left_path);
        if (i == tab_manager->active_tab) {
            attroff(COLOR_PAIR(2));
        }
    }
}

void draw_ui(TabManager* tab_manager) {
    clear();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    draw_tabs(tab_manager, 0, max_x);

    int panel_width = max_x / 2;
    int panel_height = max_y - 2;

    Tab* active_tab = &tab_manager->tabs[tab_manager->active_tab];
    draw_panel(active_tab->left_list, active_tab->left_cursor, active_tab->left_offset, 
               2, 0, panel_height, panel_width, active_tab->active_panel);
    draw_panel(active_tab->right_list, active_tab->right_cursor, active_tab->right_offset, 
               2, panel_width, panel_height, panel_width, !active_tab->active_panel);


    mvprintw(max_y - 1, 0, "F1: Switch Panel | Tab: Switch Tab | t: New Tab | q: Quit");
    refresh();
}