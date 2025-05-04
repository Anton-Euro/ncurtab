#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include "tabs.h"
#include "ui.h"


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

    UIWindows ui;
    init_ui_windows(&ui);
    
    TabManager* tab_manager = init_tabs();
    add_tab(tab_manager, getenv("HOME"));

    int ch;
    bool running = true;
    while (running) {
        draw_ui(&ui, tab_manager);

        ch = getch();
        switch (ch) {
            case KEY_RESIZE:
                resize_ui_windows(&ui);
                draw_ui(&ui, tab_manager);
                break;
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
                tab_manager->tabs[tab_manager->active_tab].active_panel = !tab_manager->tabs[tab_manager->active_tab].active_panel;
                break;
            case 'd':
                delete_entity(tab_manager, &ui);
                break;
            case 'r':
                rename_entity(tab_manager, &ui);
                break;
            case 'c':
                copy_entity_to_other_panel(tab_manager);
                break;
            case 'm':
                move_entity_to_other_panel(tab_manager);
                break;
            case 'h':
                draw_help_window(&ui);
                break;
        }
    }

    free_tabs(tab_manager);
    free_ui_windows(&ui);
    endwin();
    return 0;
}