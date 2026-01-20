#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#define EDITOR_NAME "Saiba's editor"

typedef enum state {
    RAW,
    INSERT
} editor_state;
int row, col;
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
    while (i < row)
    {
        mvprintw(i, 0,"%i", i + 1);
        i++;
    }
}
void display_state() {
    if (state == RAW)
        mvprintw(row - 1, 0, "State: normal");
    else    
        mvprintw(row - 50, 0, "State: insert");
}
void display_data() {
    display_windows_size();
    display_row_number();
    display_state();
    move(0,1);
}

int main() {
    initscr();
    getmaxyx(stdscr,row,col);		/* get the number of rows and columns */

    noecho(); // user input not displayed
    mvprintw(row/2, (col - 1 -strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);
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
