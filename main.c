#include <stdio.h>
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

void display_windows_size() {
    attron(A_BOLD);
    char windows_size[32];
    snprintf(windows_size, 32, "C:%i,R:%i", col, row); 
    mvprintw(row - 1, col - strlen(windows_size), "%s", windows_size);
    attroff(A_BOLD);
}

void display_row_number() {
    attron(A_BOLD);
    int i = 0;
    while (i < max_row)
    {
        mvprintw(i, 0,"%3i", i + 1);
        i++;
    }
}
void display_state() {
    if (state == RAW)
        mvprintw(max_row - 1, 0, "State: normal");
    else    
        mvprintw(max_row - 50, 0, "State: insert");
}
void display_data() {
    display_windows_size();
    display_row_number();
    display_state();
    move(0,1); // TO-DO: wrapper when calling move, to auto update row and col
    //row = 0; col = 1;
}

int main() {
    initscr();
    getmaxyx(stdscr,max_row,max_col);		/* get the number of rows and columns */
    noecho(); // user input not displayed
    mvprintw(max_row/2, (max_col- 1 -strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);
    display_data();
    refresh();
    while (1) {
        int input = getch();
        int loop_state = 1;
        switch(input) {
            case 'q':
                loop_state = 0;
                break;
            default:
                break;
        }
        if (!loop_state)
            break;
        clear();
        display_data();
        refresh();
    }
    endwin(); // close the screen
    return 0;
}
