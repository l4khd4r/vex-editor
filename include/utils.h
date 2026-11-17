#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "vex.h"

#define KEY_CHARACTER 1
#define KEY_ENTER 2
#define KEY_ARROW 3
#define KEY_QUIT 4
#define KEY_BACKSPACE 5
#define KEY_SAVE 6

void insert_char(vex_t *editor , char c ) {
    // Implementation for inserting a character
    editor->buffer[editor->bytes_read] = c ;
    editor->bytes_read++ ;
    if(editor->bytes_read >= editor->buffer_size) {
        editor->buffer_size *= 2 ;
        editor->buffer = realloc(editor->buffer , editor->buffer_size) ;
        if(editor->buffer == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
}

void insert_newline(vex_t *editor) {
    // Implementation for inserting a newline
    insert_char(editor, '\r');
    insert_char(editor, '\n');
}

void move_cursor(vex_t *editor , char arrow) {
    // Implementation for moving the cursor based on arrow key input
    switch(arrow) {
        case 'w' :
        case 'W' :
        case 'k' :
        case 'K': // Up
            // Move cursor up logic
            if(editor->cursor_y > 0) {
                editor->cursor_y--;
            }
            break;
        case 's' : 
        case 'S' : 
        case 'j' : 
        case 'J': // Down
            // Move cursor down logic
            if(editor->cursor_y < 24) {
                editor->cursor_y++;
            }
            break;
        case 'd' : 
        case 'D' : 
        case 'l' : 
        case 'L': // Right
            // Move cursor right logic
            if(editor->cursor_x < 79) {
                editor->cursor_x++;
            }
            break;
        case 'a' : 
        case 'A' : 
        case 'h' : 
        case 'H': // Left
            // Move cursor left logic
            if(editor->cursor_x > 0) {
                editor->cursor_x--;
            }
            break;
    }
}

void delete_char(vex_t *editor) {
    if(editor->bytes_read > 0) {
        editor->bytes_read-- ;
        editor->buffer[editor->bytes_read] = '\0' ;
    }
}


void save_file(vex_t *editor) {
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fwrite(editor->buffer, sizeof(char), editor->bytes_read, file);
    fclose(file);
}

void refresh_screen(vex_t *editor) {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen

    // Move the cursor to the top-left corner
    write(STDOUT_FILENO, "\x1b[H", 3);

    // Write the buffer content to the screen
    write(STDOUT_FILENO, editor->buffer, editor->bytes_read);
}
vex_key_t read_key() {
    char c ; 
    vex_key_t key ;

    read(STDIN_FILENO , &c, 1 ) ;
    key.c = c ;

    if(c == '\r') {
        key.type = KEY_ENTER ;
    } else if(c == 127) {
        key.type = KEY_BACKSPACE ;
    } else if(c == 19) {
        key.type = KEY_SAVE ;
    } else if(c == 17) {
        key.type = KEY_QUIT ;
    } else if(c == '\x1b') {
        char seq[3] ;
        if(read(STDIN_FILENO , &seq[0] , 1) != 1) {
            key.type = KEY_CHARACTER ;
            return key ;
        }
        if(read(STDIN_FILENO , &seq[1] , 1) != 1) {
            key.type = KEY_CHARACTER ;
            return key ;
        }
        if(seq[0] == '[') {
            switch(seq[1]) {
                case 'A':
                    key.type = KEY_ARROW ;
                    key.arrow = 'w' ; // Up
                    break;
                case 'B':
                    key.type = KEY_ARROW ;
                    key.arrow = 's' ; // Down
                    break;
                case 'C':
                    key.type = KEY_ARROW ;
                    key.arrow = 'd' ; // Right
                    break;
                case 'D':
                    key.type = KEY_ARROW ;
                    key.arrow = 'a' ; // Left
                    break;
                default:
                    key.type = KEY_CHARACTER ;
                    break;
            }
        } else {
            key.type = KEY_CHARACTER ;
        }
    } else {
        key.type = KEY_CHARACTER ;
    }
    return key ;
}
#endif