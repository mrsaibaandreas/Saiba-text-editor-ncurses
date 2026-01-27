#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include "logger.h"

#define EDITOR_NAME "Saiba's editor"

typedef enum editor_state {
    RAW,
    INSERT
} editor_state;

typedef struct editor_s{
    WINDOW *win;
    int col;
    int row;
} editor_s;

void init_window(editor_s *window, int ncol, int nlines, int begin_y, int begin_x); 

int row = 0; int col = 0;
int max_row, max_col;

editor_state state = RAW;

editor_s main_w = {NULL, 0,0}; // TO-DO make every struct initialized like this
editor_s  row_w, info_w;

// Callbacks for updating screen data
void onInfoUpdate(); 
void onRowUpdate();

void display_windows_size();
void display_row_number();
void display_data(); 

void init_screen(); // this functions initialize screen related structures

int main() {
    INIT_LOG("log/test/");
    init_screen();
    int input; 
    clear();
    LOG_D("Logging of editor initialized");
    while ((input = getch()) != 'q') {
        switch(input) {
            case KEY_UP:
                if (main_w.row == 0)
                    break;
                main_w.row--;
                break;
            case KEY_DOWN:
                if (main_w.row == max_row)
                    break;
                main_w.row++;
                break;
            case KEY_RIGHT:
                if (main_w.col == max_col)
                    break;
                main_w.col++;
                break;
            case KEY_LEFT:
                if (main_w.col == 0)
                    break;
                main_w.col--;
                break;
            default:
                
                LOG_I(WARNING, "Inconsistent area, key %i pressed", input);
                break;
        }
        refresh();
        wrefresh(main_w.win);
        mvwprintw(main_w.win, max_row/2, (max_col - 1)/2, "%i %i", main_w.row, main_w.col);
        display_data();
    }
    endwin(); // close the screen
    return 0;
}

void init_window(editor_s *window, int ncol, int nlines, int begin_y, int begin_x) {
    window->win = newwin(ncol, nlines, begin_y, begin_x);
    window->col = ncol - 1;
    window->row = nlines - 1;
    LOG_D("Window init with %i %i", window->col, window->row);
}

void init_screen() {
    initscr();
    getmaxyx(stdscr,max_row,max_col);		/* get the number of rows and columns */
    noecho(); // user input not displayed
    cbreak();
    keypad(stdscr, TRUE);
    init_window(&main_w, max_row - 2, max_col -4, 0, 3);
    init_window(&row_w, max_row - 1, 3, 0, 0);
    init_window(&info_w, 1, max_col, max_row - 1, 0);
    mvprintw(win, max_row/2, (max_col - 1 - strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);
    display_data();
    wrefresh(main_w.win);
}

void display_windows_size() {
    attron(A_BOLD);
    if (state == RAW)
        mvwprintw(info_w.win, 0, 0, "State: normal");
    else    
        mvwprintw(info_w.win, 0, 0, "State: insert");
    char windows_size[32];
    snprintf(windows_size, 32, "R:%i C:%i, %i:%i", max_row, max_col, row, col); 
    mvwprintw(info_w.win, 0, max_col - strlen(windows_size), "%s", windows_size);
    LOG_D("INFO_W row: %i col: %i", info_w.row, info_w.col);
    attroff(A_BOLD);
    wrefresh(info_w.win);
}

void display_row_number() {
    wattron(row_w.win, A_BOLD);
    int i = 0;
    while (i < max_row)
    {
        mvwprintw(row_w.win, i, 0,"%3i", i + 1);
        i++;
    }
    LOG_D("ROW_W row:%i col:%i", row_w.row, row_w.col);
    wrefresh(row_w.win);
    wattroff(row_w.win, A_BOLD);
}

void display_data() {
    display_windows_size();
    display_row_number();
    wmove(main_w.win, main_w.row, main_w.col);// TO-DO: wrapper when calling move, to auto update row and col
    printf("%i %i\n", main_w.row, main_w.col);
    wrefresh(main_w.win);
}
