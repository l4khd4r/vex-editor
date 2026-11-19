#include "terminal.h"

#include <ctype.h>
#include <stdio.h>

#define CTRL_KEY(k) ((k) & 0x1f)

int main(void) {
    terminal_t term  = {0};

    if(terminal_init(&term) == -1) {
        return 1;
    }

    if(terminal_enable_raw_mode(&term) == -1) {
        return 1;
    }

    terminal_clear();
    if(terminal_get_size(&term) == -1) {
        printf("Unable to get terminal size\r\n");
    }else {
        printf("size: %zu rows x %zu cols\r\n" , term.rows , term.cols);
    }
    
    for(;;) {
        int c = terminal_read_key() ;
        if(c == -1 || c == CTRL_KEY('q'))
            break ;
        
        if(c >= 1000) {
            printf("special key: %d\r\n" , c);
        }else if(iscntrl(c)) 
            printf("%d\r\n", c);
        else
            printf("%d ('%c')\r\n", c , c);
    }
    terminal_destroy(&term);
    return 0;
}