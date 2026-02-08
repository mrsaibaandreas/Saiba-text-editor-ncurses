#ifndef NCRUSES_E_GAP_H
#define NCRUSES_E_GAP_H

#define GAP_BUF_SIZE 1024
#define GAP_SIZE 100
typedef struct _gap_buffer {
    char gap_buffer[GAP_BUF_SIZE];
    int gap_start;
    int gap_end;
    int cursor_position;

} gap_buffer;

void insert_character(char character);
void insert_string(char *char_string);
void delete_character();

#endif
