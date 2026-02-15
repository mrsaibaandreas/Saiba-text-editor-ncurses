// add newline for removing leftovers at each refrsh
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
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

typedef struct _file_info {
    int file_size_in_bytes;
    int file_nr_of_rows;
    int file_nr_of_characters;
} file_info;

int row = 0; int col = 0;
int load_first_time_up = 1;
int max_row, max_col;
int fd;

char file_buffer[FILE_CHUNCK_BUFFER_SIZE];

editor_state state = RAW;

editor_s main_w, row_w, info_w;

file_info info;

/* Auxialiaries */
void help() {
    printf("Insufficient arguments\n");
    printf("Usage: editor <file>\n");
}
/* End of auxiliaries */

/* File manipulation functions */
void open_file(char *filename);
int read_file();
void get_file_info(file_info *info);
/* End of file manipulation functions */

/* Functions for manipulating the screen */
void display_windows_size();
void display_row_number();
void display_file_contents();
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
    get_file_info(&info);
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
        display_data();
    }
    endwin(); // close the screen
    close(fd);
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

    init_window(&main_w, max_row - 1, max_col - 5, 0, 3);
    init_window(&row_w, max_row - 1, 4, 0, 0);
    init_window(&info_w, 1, max_col, max_row - 1, 0);

    keypad(main_w.win, TRUE);

    mvwprintw(main_w.win, max_row/2, (max_col - 1 - strlen(EDITOR_NAME))/2, "%s", EDITOR_NAME);
    main_w.row = 0; main_w.col = 0;

    display_data();
    load_first_time_up = 0;
    wrefresh(main_w.win);
}

void display_windows_size() {
    wclear(info_w.win);
    attron(A_BOLD);
    if (state == RAW)
        mvwprintw(info_w.win, 0, 0, "State: normal");
    else    
        mvwprintw(info_w.win, 0, 0, "State: insert");

    char windows_size[64];
    snprintf(windows_size, 64, "R:%i C:%i, %i:%i\n", max_row - 1, max_col - 4, main_w.row, main_w.col); 

    mvwaddnstr(info_w.win, 0, max_col - strlen(windows_size), windows_size, strlen(windows_size));

    attroff(A_BOLD);
    clear();
    wrefresh(info_w.win);
}

void display_row_number() {
    wattron(row_w.win, A_BOLD);

    int i = 0;
    while (i < max_row)
    {
        mvwprintw(row_w.win, i, 0,"%2i ", i + 1);
        i++;
    }

    wrefresh(row_w.win);

    wattroff(row_w.win, A_BOLD);
}

void display_file_contents() {
    if (load_first_time_up || main_w.row == main_w.row - 1) {
        read_file();
    }
    // we'll print everytime at 0 0 because no matter what line
    // the cursor is, the window does not care about the line
    mvwprintw(main_w.win, 0, 0, "%s", file_buffer); 
}

void display_data() {
    display_windows_size();
    display_row_number();
        display_file_contents();
    wmove(main_w.win, main_w.row, main_w.col);// TO-DO: wrapper when calling move, to auto update row and col
    wrefresh(main_w.win);
}


void open_file(char *filename) {
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // study more about file rights at creation
    if (fd == -1) {
        fprintf(stderr, "Failed to open the file %s\n", filename);
        LOG_I(INFO, "Failed to open the file %s", filename);
        endwin();
        exit(-1);
    }
}

int read_file() {
   int size = read(fd, file_buffer, FILE_CHUNCK_BUFFER_SIZE -  1);
   file_buffer[FILE_CHUNCK_BUFFER_SIZE] = '\0';

   if (size == 0) {
       LOG_I(INFO, "End of file reached, no more input to parse");
       return 0;
   }

   if (size == -1) {
        LOG_I(ERROR, "File read() error");
        perror("File read() error");
        // in the future this needs to changed, it should just print an error
        // and inform the user that the file cannot be read
        endwin();
        exit(EXIT_FAILURE); 
   }
   return size;
}

void get_file_info(file_info *info) {
    struct stat sb;

    if (fstat(fd, &sb) == -1) {
        perror("File stat() error");
        endwin();
        exit(EXIT_FAILURE);
    }

    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        perror("Not a regular file");
        endwin();
        exit(EXIT_FAILURE);
    }
    
    info->file_size_in_bytes = sb.st_size;

    int lines = 0;
    lseek(fd, 0, SEEK_SET);
    char get_file_info_buffer[FILE_CHUNCK_BUFFER_SIZE]; 
    while (read(fd, get_file_info_buffer, FILE_CHUNCK_BUFFER_SIZE - 1)) {
        file_buffer[FILE_CHUNCK_BUFFER_SIZE] = '\0';
        int count = 0;
        while (get_file_info_buffer[count] != '\0') {
            if (get_file_info_buffer[count++] == '\n') lines++;
        }
    }

    info->file_nr_of_rows = lines;
    lseek(fd, 0, SEEK_SET);
}
