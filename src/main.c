#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include "ui.h"
#include "tabs.h"

#define MAX_PATH 256

int main() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    TabManager* tab_manager = init_tabs();
    add_tab(tab_manager, "/");

    int ch;
    bool running = true;
    while (running) {
        draw_ui(tab_manager);

        ch = getch();
        switch (ch) {
            case 'q':
                running = false;
                break;
            case 't':
                add_tab(tab_manager, tab_manager->tabs[tab_manager->active_tab].left_path);
                break;
            case 9: // Tab key
                switch_tab(tab_manager, (tab_manager->active_tab + 1) % tab_manager->tab_count);
                break;
            case KEY_UP:
                move_cursor(&tab_manager->tabs[tab_manager->active_tab], -1);
                break;
            case KEY_DOWN:
                move_cursor(&tab_manager->tabs[tab_manager->active_tab], 1);
                break;
            case 10: // Enter key
                enter_directory(tab_manager);
                break;
            case KEY_F(1):
                tab_manager->tabs[tab_manager->active_tab].active_panel = 
                    !tab_manager->tabs[tab_manager->active_tab].active_panel;
                break;
            case KEY_DC: // Delete key
                delete_file_or_dir(tab_manager);
                break;
            case KEY_F(2): // F2 for rename
                rename_file_or_dir_ui(tab_manager);
                break;
            case KEY_F(5): // F5 for copy
                copy_file_to_other_panel(tab_manager);
                break;
            case KEY_F(6): // F6 for move
                move_file_to_other_panel(tab_manager);
                break;
        }
    }

    free_tabs(tab_manager);
    endwin();
    return 0;
}