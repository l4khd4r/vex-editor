#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* atexit() can't pass arguments, so remember the active terminal here */


static terminal_t *g_active_terminal = NULL ;

static bool g_atexit_registered = false;

static void terminal_restore_at_exit(void) {
    if(g_active_terminal) 
        terminal_disable_raw_mode(g_active_terminal) ;
}

int terminal_init(terminal_t *term) {
    if(!term)
        return -1;
    
    if(term->initialized) // already initialized no need to overwrite
        return 0;
    
    if(!isatty(STDIN_FILENO)) {
        fprintf(stderr, "vex: standard input is not a terminal\n");
        return -1;
    }
    
    if(tcgetattr(STDIN_FILENO , &term->original_termios) == -1 ) {
        perror("tcgetattr");
        return -1 ;
    }
    term->current_termios = term->original_termios ;
    term->rows = 0 ;
    term->cols = 0 ;
    term->raw_mode = false;
    term->initialized = true ;
    return 0;

}


int terminal_enable_raw_mode(terminal_t *term) {
    if(!term || !term->initialized)
        return -1;
    if(term->raw_mode)
        return 0;
    
    if(!g_atexit_registered) {
        if(atexit(terminal_restore_at_exit) != 0) {
            fprintf(stderr , "vex: atexit failed\n");
            return -1 ;
        }
        g_atexit_registered = true;
    }

    struct termios raw = term->current_termios ;

    raw.c_iflag &= ~(BRKINT |  ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST); /*now we must print crnl yourself*/
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0; // read() may return with 0 bytes 
    raw.c_cc[VTIME] = 1; // timeout read after 100ms 

    if(tcsetattr(STDIN_FILENO , TCSAFLUSH , &raw) == -1) {
        perror("tcsetattr");
        return -1 ;
    }

    // flip state only after success 
    term->current_termios = raw ;
    term->raw_mode = true ;
    g_active_terminal = term ;
    return 0 ;
}

int terminal_disable_raw_mode(terminal_t *term) {
    if(!term || !term->initialized) {
        return -1 ;
    }
    if(!term->raw_mode) {
        return 0;
    }

    if(tcsetattr(STDIN_FILENO , TCSAFLUSH , &term->original_termios) == -1 ) {
        perror("tcsetattr");
        return -1 ;
    }
    term->current_termios = term->original_termios ;
    term->raw_mode = false;
    return 0 ;
}


void terminal_destroy(terminal_t *term) {
    if(!term || !term->initialized )
        return ;

    terminal_disable_raw_mode(term) ;
    if(g_active_terminal == term)
        g_active_terminal = NULL ;
    
    term->initialized = false ;
}
