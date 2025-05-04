#include "ui.h"

void init_ui_windows(UIWindows* ui) {
    getmaxyx(stdscr, ui->max_y, ui->max_x);

    resize_ui_windows(ui);
}

void resize_ui_windows(UIWindows* ui) {
    system("clear");
    
    if (ui->tabs_win) delwin(ui->tabs_win);
    if (ui->panels_container_win) delwin(ui->panels_container_win);
    if (ui->left_panel_win) delwin(ui->left_panel_win);
    if (ui->right_panel_win) delwin(ui->right_panel_win);
    if (ui->status_win) delwin(ui->status_win);

    getmaxyx(stdscr, ui->max_y, ui->max_x);

    ui->tabs_win = newwin(1, ui->max_x, 0, 0);
    ui->status_win = newwin(1, ui->max_x, ui->max_y - 1, 0);
    int panel_height = ui->max_y - 2;
    int panel_width = ui->max_x / 2;
    ui->panels_container_win = newwin(panel_height, ui->max_x, 1, 0);

    ui->left_panel_win = newwin(panel_height - 2, panel_width - 2, 2, 1);
    ui->right_panel_win = newwin(panel_height - 2, panel_width - 1, 2, panel_width);

    refresh();
}

void draw_panel(WINDOW* win, FileList* list, int cursor, int offset, int height, int width, bool active) {
    werase(win);

    int max_name_len = width - 4;

    for (int i = 0; i < height - 2 && (i + offset) < list->count; i++) {
        int idx = i + offset;
        bool is_selected = (idx == cursor && active);

        char display_name[MAX_NAME];
        strncpy(display_name, list->entries[idx].name, MAX_NAME - 2);
        display_name[MAX_NAME - 2] = '\0';

        if (list->entries[idx].is_dir) {
            strncat(display_name, "/", MAX_NAME - strlen(display_name) - 1);
        }

        if (strlen(display_name) > max_name_len) {
            display_name[max_name_len - 3] = '.';
            display_name[max_name_len - 2] = '.';
            display_name[max_name_len - 1] = '.';
            display_name[max_name_len] = '\0';
        }

        if (list->entries[idx].is_dir) {
            wattron(win, COLOR_PAIR(3));
        } else {
            wattron(win, COLOR_PAIR(4));
        }

        if (is_selected) {
            wattron(win, A_REVERSE);
        }

        mvwprintw(win, i, 1, "%-*s", max_name_len, display_name);

        if (is_selected) {
            wattroff(win, A_REVERSE);
        }
        if (list->entries[idx].is_dir) {
            wattroff(win, COLOR_PAIR(3));
        } else {
            wattroff(win, COLOR_PAIR(4));
        }
    }
    wrefresh(win);
}

void draw_tabs(WINDOW* win, TabManager* tab_manager) {
    werase(win);
    for (int i = 0; i < tab_manager->tab_count; i++) {
        if (i == tab_manager->active_tab) {
            wattron(win, COLOR_PAIR(2));
        }
        mvwprintw(win, 0, i * 10, "Tab %d", i + 1);
        if (i == tab_manager->active_tab) {
            wattroff(win, COLOR_PAIR(2));
        }
    }
    wrefresh(win);
}


void draw_ui(UIWindows* ui, TabManager* tab_manager) {
    Tab* active_tab = &tab_manager->tabs[tab_manager->active_tab];

    draw_tabs(ui->tabs_win, tab_manager);

    werase(ui->panels_container_win);
    box(ui->panels_container_win, 0, 0);
    char left_truncated[MAX_PATH];
    char right_truncated[MAX_PATH];
    strncpy(left_truncated, active_tab->left_path, ui->max_x / 2 - 3);
    left_truncated[ui->max_x / 2 - 3] = '\0';
    strncpy(right_truncated, active_tab->right_path, ui->max_x / 2 - 3);
    right_truncated[ui->max_x / 2 - 3] = '\0';

    mvwprintw(ui->panels_container_win, 0, 2, "%s", left_truncated);
    mvwprintw(ui->panels_container_win, 0, ui->max_x / 2 + 2, "%s", right_truncated);
    
    int separator_x = ui->max_x / 2 - 1;
    mvwaddch(ui->panels_container_win, 0, separator_x, ACS_TTEE);
    for (int y = 1; y < ui->max_y - 3; y++) {
        mvwaddch(ui->panels_container_win, y, separator_x, ACS_VLINE);
    }
    mvwaddch(ui->panels_container_win, ui->max_y - 3, separator_x, ACS_BTEE);
    
    wrefresh(ui->panels_container_win);

    int panel_height = ui->max_y - 2;
    int panel_width = ui->max_x / 2;

    draw_panel(ui->left_panel_win, active_tab->left_list, active_tab->left_cursor, 
               active_tab->left_offset, panel_height, panel_width, active_tab->active_panel);

    draw_panel(ui->right_panel_win, active_tab->right_list, active_tab->right_cursor, 
               active_tab->right_offset, panel_height, panel_width, !active_tab->active_panel);

}

void draw_help_window(UIWindows* ui) {
    int help_width = 50;
    int help_height = 17;
    int start_y = (ui->max_y - help_height) / 2;
    int start_x = (ui->max_x - help_width) / 2;

    WINDOW* help_win = newwin(help_height, help_width, start_y, start_x);
    box(help_win, 0, 0);
    mvwprintw(help_win, 1, 2, "Command Reference");
    mvwaddch(help_win, 2, 0, ACS_LTEE);
    for (int x = 1; x < help_width - 1; x++) {
        mvwaddch(help_win, 2, x, ACS_HLINE);
    }
    mvwaddch(help_win, 2, help_width - 1, ACS_RTEE);
    mvwprintw(help_win, 4, 2, "q          : Quit the program");
    mvwprintw(help_win, 5, 2, "t          : New tab");
    mvwprintw(help_win, 6, 2, "Tab        : Switch to next tab");
    mvwprintw(help_win, 7, 2, "Up/Down    : Move cursor");
    mvwprintw(help_win, 8, 2, "Enter      : Enter directory");
    mvwprintw(help_win, 9, 2, "F1         : Switch panel");
    mvwprintw(help_win, 10, 2, "d          : Delete file/directory");
    mvwprintw(help_win, 11, 2, "r          : Rename file/directory");
    mvwprintw(help_win, 12, 2, "c          : Copy to other panel");
    mvwprintw(help_win, 13, 2, "m          : Move to other panel");
    mvwprintw(help_win, 14, 2, "h          : Show this help");
    mvwprintw(help_win, 16, 2, "Press any key to close");
    wrefresh(help_win);

    getch();
    delwin(help_win);

    touchwin(ui->tabs_win);
    touchwin(ui->left_panel_win);
    touchwin(ui->right_panel_win);
    touchwin(ui->status_win);
    wrefresh(ui->tabs_win);
    wrefresh(ui->left_panel_win);
    wrefresh(ui->right_panel_win);
    wrefresh(ui->status_win);
}

bool prompt_new_name(UIWindows* ui, const char* old_name, char* new_name) {
    int rename_width = 50;
    int rename_height = 7;
    int start_y = (ui->max_y - rename_height) / 2;
    int start_x = (ui->max_x - rename_width) / 2;

    WINDOW* rename_win = newwin(rename_height, rename_width, start_y, start_x);
    box(rename_win, 0, 0);
    mvwprintw(rename_win, 1, 2, "Rename File/Directory");
    mvwaddch(rename_win, 2, 0, ACS_LTEE);
    for (int x = 1; x < rename_width - 1; x++) {
        mvwaddch(rename_win, 2, x, ACS_HLINE);
    }
    mvwaddch(rename_win, 2, rename_width - 1, ACS_RTEE);
    mvwprintw(rename_win, 3, 2, "Current name: %s", old_name);
    mvwprintw(rename_win, 4, 2, "New name: ");
    mvwprintw(rename_win, 6, 2, "Enter to confirm, Esc to cancel");

    echo();
    curs_set(1);
    wmove(rename_win, 4, 12);
    wrefresh(rename_win);

    char input[MAX_NAME];
    int ch;
    int i = 0;
    input[0] = '\0';

    while ((ch = wgetch(rename_win)) != 10 && ch != 27) {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                input[i] = '\0';
                mvwprintw(rename_win, 4, 12, "%-*s", rename_width - 14, input);
                wmove(rename_win, 4, 12 + i);
                wrefresh(rename_win);
            }
        } else if (i < MAX_NAME - 1 && ch >= 32 && ch <= 126) {
            input[i++] = ch;
            input[i] = '\0';
            mvwprintw(rename_win, 4, 12, "%s", input);
            wmove(rename_win, 4, 12 + i);
            wrefresh(rename_win);
        }
    }

    noecho();
    curs_set(0);
    delwin(rename_win);

    if (ch == 27 || i == 0) {
        touchwin(ui->tabs_win);
        touchwin(ui->left_panel_win);
        touchwin(ui->right_panel_win);
        touchwin(ui->status_win);
        wrefresh(ui->tabs_win);
        wrefresh(ui->left_panel_win);
        wrefresh(ui->right_panel_win);
        wrefresh(ui->status_win);
        return false;
    }

    strncpy(new_name, input, MAX_NAME);
    new_name[MAX_NAME - 1] = '\0';

    touchwin(ui->tabs_win);
    touchwin(ui->left_panel_win);
    touchwin(ui->right_panel_win);
    touchwin(ui->status_win);
    wrefresh(ui->tabs_win);
    wrefresh(ui->left_panel_win);
    wrefresh(ui->right_panel_win);
    wrefresh(ui->status_win);
    return true;
}

bool confirm_delete(UIWindows* ui, const char* name) {
    int delete_width = 50;
    int delete_height = 6;
    int start_y = (ui->max_y - delete_height) / 2;
    int start_x = (ui->max_x - delete_width) / 2;

    WINDOW* delete_win = newwin(delete_height, delete_width, start_y, start_x);
    box(delete_win, 0, 0);
    mvwprintw(delete_win, 1, 2, "Confirm Deletion");
    mvwaddch(delete_win, 2, 0, ACS_LTEE);
    for (int x = 1; x < delete_width - 1; x++) {
        mvwaddch(delete_win, 2, x, ACS_HLINE);
    }
    mvwaddch(delete_win, 2, delete_width - 1, ACS_RTEE);
    mvwprintw(delete_win, 3, 2, "Delete %s? (y/n)", name);
    mvwprintw(delete_win, 5, 2, "y: Yes, n: No");
    wrefresh(delete_win);

    int ch;
    do {
        ch = getch();
    } while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N');

    delwin(delete_win);

    touchwin(ui->tabs_win);
    touchwin(ui->left_panel_win);
    touchwin(ui->right_panel_win);
    touchwin(ui->status_win);
    wrefresh(ui->tabs_win);
    wrefresh(ui->left_panel_win);
    wrefresh(ui->right_panel_win);
    wrefresh(ui->status_win);

    return (ch == 'y' || ch == 'Y');
}

void free_ui_windows(UIWindows* ui) {
    delwin(ui->tabs_win);
    delwin(ui->panels_container_win);
    delwin(ui->left_panel_win);
    delwin(ui->right_panel_win);
    delwin(ui->status_win);
    system("clear");
}