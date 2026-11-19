#ifndef TERMINAL_H
#define TERMINAL_H
#include <termios.h>
#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>





typedef struct terminal_s  { 
    struct termios original_termios ;
    struct termios current_termios ;
    
    size_t rows ;
    size_t cols ;
    
    bool raw_mode ;
    bool initialized ;
} terminal_t ;
// keys like -> up reight .... , are not an ascci coded so we will define them here as an enum type so we can use them in our code
enum editor_key {
    KEY_ESC = 27,
    KEY_BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
}; 


int  terminal_init(terminal_t *term);
void terminal_destroy(terminal_t *term);


int terminal_enable_raw_mode(terminal_t *term);
int terminal_disable_raw_mode(terminal_t *term);

// we will think about this later 
// int terminal_get_size(terminal_t *term);

int terminal_read_key(void);

int terminal_write(const char *buffer, size_t len);
int terminal_clear(void);


#endif