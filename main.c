#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#include <fcntl.h>

#include "logger.h"

#define EDITOR_NAME "Saiba's editor"
#define FILE_CHUNCK_BUFFER_SIZE 1024
typedef enum editor_state {
    RAW,
    INSERT
} editor_state;

typedef struct editor_s{
    WINDOW *win;
    int row, col;
    int max_row, max_col;
} editor_s;


int row = 0; int col = 0;

int max_row, max_col;
int fd;

char file_buffer[FILE_CHUNCK_BUFFER_SIZE];

editor_state state = RAW;

editor_s main_w, row_w, info_w;


/* Auxialiaries */
void help() {
    printf("Insufficient arguments\n");
    printf("Usage: editor <file>\n");
}
/* End of auxiliaries */

/* File manipulation function */
void open_file(char *filename);
int read_file();
/* End of file manipulation functions */
// Callbacks for updating screen data
void onInfoUpdate(); 
void onRowUpdate();

/* Functions for manipulating the screen */
void display_windows_size();
void display_row_number();
void display_data(); 

void init_window(editor_s *window, int ncol, int nlines, int begin_y, int begin_x); 
void init_screen(); // this functions initialize screen related structures
/* End of functions manipulating the screen */

int main(int argc, char **argv) {
    
    if (argc == 1) {
        help();
        exit(-1); // Transfer the code to work with errno
    }
     
    INIT_LOG("log/test/");
    
    open_file(argv[1]); 

    init_screen();
    int input; 
    clear();
    LOG_D("Logging of editor initialized");
    while ((input = wgetch(main_w.win)) != 'q') {
        switch(input) {
            case KEY_UP:
                if (main_w.row - 1 == -1)
                    break;
                main_w.row--;
                break;
            case KEY_DOWN:
                if (main_w.row + 1 == main_w.max_row) 
                    break;
                main_w.row++;
                break;
            case KEY_RIGHT:
                if (main_w.col + 1 == main_w.max_col)
                    break;
                main_w.col++;
                break;
            case KEY_LEFT:
                if (main_w.col - 1 == -1)
                    break;
                main_w.col--;
                break;
            default:
                LOG_I(WARNING, "Inconsistent area, key %i pressed", input);
                break;
        }
        refresh();
#ifdef DEBUG_A
        mvwprintw(main_w.win, 40, 40, "Max_row: %i, Max_col:%i, Cur_row:%i, Cur_col:%i",\
                main_w.max_row, main_w.max_col, main_w.row, main_w.col);
#endif
        display_data();
    }
    endwin(); // close the screen
    return 0;
}

void init_window(editor_s *window, int nlines, int ncol, int begin_y, int begin_x) {
    window->win = newwin(nlines, ncol, begin_y, begin_x);
    window->col = 0;
    window->row = 0;
    window->max_col = ncol;
    window->max_row = nlines;
    LOG_D("Window init with %i %i", window->col, window->row);
}

void init_screen() {
    initscr();
    clear();
    getmaxyx(stdscr,max_row,max_col);		/* get the number of rows and columns */
    noecho(); // user input not displayed
    cbreak();

    init_window(&main_w, max_row - 1, max_col - 4, 0, 3);
    init_window(&row_w, max_row - 1, 3, 0, 0);
    init_window(&info_w, 1, max_col, max_row - 1, 0);

    keypad(main_w.win, TRUE);

    mvwprintw(main_w.win, max_row/2, (max_col - 1 - strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);

    display_data();

    wrefresh(main_w.win);
}

void display_windows_size() {
    attron(A_BOLD);
    if (state == RAW)
        mvwprintw(info_w.win, 0, 0, "State: normal");
    else    
        mvwprintw(info_w.win, 0, 0, "State: insert");

    char windows_size[64];
    snprintf(windows_size, 64, "R:%i C:%i, %i:%i", max_row - 1, max_col - 4, main_w.row, main_w.col); 

    mvwprintw(info_w.win, 0, max_col - strlen(windows_size), "%s", windows_size);

    attroff(A_BOLD);
    clear();
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

    wrefresh(row_w.win);

    wattroff(row_w.win, A_BOLD);
}

void display_data() {
    display_windows_size();
    display_row_number();

    wmove(main_w.win, main_w.row, main_w.col);// TO-DO: wrapper when calling move, to auto update row and col

    wrefresh(main_w.win);
}


void open_file(char *filename) {
    fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // study more about file rights at creation
    if (fd == -1) {
        fprintf(stderr, "Failed to open the file\n");
        LOG_I(INFO, "Failed to open the file");
        exit(-1);
    }
}
