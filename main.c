#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#define EDITOR_NAME "Saiba's editor"

typedef enum state {
    RAW,
    INSERT
} editor_state;

int row = 0; int col = 0;
int max_row, max_col;

editor_state state = RAW;

WINDOW *main_w, *row_w, *info_w;

// Callbacks for updating screen data
void onInfoUpdate(); 
void onRowUpdate();

void display_windows_size();
void display_row_number();
void display_state(); 
void display_data(); 

void init_screen(); // this functions initialize screen related structures

int main() {
    init_screen();
    int input = wgetch(main_w);
    clear();
    while (1) {
        int loop_state = 1;
        switch(input) {
            case KEY_UP:
                if (row == 0)
                    break;
                row--;
                break;
            case KEY_DOWN:
                if (row == max_row)
                    break;
                row++;
                break;
            case KEY_RIGHT:
                if (col == max_col)
                    break;
                col++;
                break;
            case KEY_LEFT:
                if (col == 0)
                    break;
                col--;
                break;
            case 'q':
                loop_state = 0;
                break;
            default:
                mvwprintw(main_w, max_row -1, 10, "WHat5 the hell");
                break;
        }
        if (!loop_state)
            break;
        display_data();
        refresh();
    mvwprintw(main_w, max_row - 10, (max_col- 1 -strlen(EDITOR_NAME))/2, "%i %i", row, col);
    wrefresh(main_w);
        input = wgetch(main_w);
    }
    endwin(); // close the screen
    return 0;
}
void init_screen() {
    initscr();
    getmaxyx(stdscr,max_row,max_col);		/* get the number of rows and columns */
    noecho(); // user input not displayed
    cbreak();
    keypad(main_w, TRUE);
    main_w = newwin(max_row - 1,  max_col - 4, 0, 3);
    row_w = newwin(max_row - 1, 3, 0 ,0);
    info_w = newwin(1, max_col - 1, max_row - 1, 0); 
    mvwprintw(main_w, max_row/2, (max_col- 1 -strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);

    display_data();
    wrefresh(main_w);
}

void display_windows_size() {
    attron(A_BOLD);
    char windows_size[32];
    snprintf(windows_size, 32, "R:%i C:%i, %i:%i", max_row, max_col, row, col); 
    mvprintw(max_row - 1, max_col - strlen(windows_size), "%s", windows_size);
    attroff(A_BOLD);
}

void display_row_number() {
    attron(A_BOLD);
    int i = 0;
    while (i < max_row)
    {
        mvwprintw(row_w, i, 0,"%3i", i + 1);
        i++;
    }
    wrefresh(row_w);
}
void display_state() {
    if (state == RAW)
        mvwprintw(main_w, max_row - 1, 0, "State: normal");
    else    
        mvwprintw(main_w, max_row - 50, 0, "State: insert");
    wrefresh(main_w);
}
void display_data() {
    display_windows_size();
    display_row_number();
    display_state();
    wmove(main_w, row, col);// TO-DO: wrapper when calling move, to auto update row and col
    wrefresh(main_w);
}
