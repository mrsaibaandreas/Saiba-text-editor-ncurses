#ifndef NCRUSES_E_GAP_H
#define NCRUSES_E_GAP_H

typedef struct _file_info {
    int file_size_in_bytes;
    int file_nr_of_rows;
    int file_nr_of_characters;
} file_info;
/* File manipulation function */
static void open_file(char *filename);

static int read_file(int fd);

static int get_file_info(int fd, file_info *info);

#endif
