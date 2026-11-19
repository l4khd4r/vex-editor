#include "terminal.h"


#include <sys/ioctl.h>
#include <unistd.h>

static int get_cursor_position(int *rows , int *cols) {
    char buf[32];
    unsigned int i = 0;

    if(write(STDOUT_FILENO , "\x1b[6n" , 4) != 4)
        return -1 ;
    while(i < sizeof(buf) -1 ) {
        if(read_byte((unsigned char*)&buf[i]) != 1)
            break ;
        if(buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';
    if(buf[0] != '\x1b' || buf[1] != '[')
        return -1 ;
    
    if(sscanf(&buf[2] , "%d;%d", rows , cols ) != 2)
        return -1 ;
    return 0;
}
int terminal_get_size(terminal_t *term) {

    struct winsize ws;
    if(!term || !term->initialized)
        return -1;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0 || ws.ws_row == 0)
    {

        if(!term->raw_mode)
            return -1;
        
        if(terminal_write("\x1b[999C\x1b[999B", 12)== -1)
            return -1;
        
        int rows, cols ;
        if(get_cursor_position(&rows , &cols) == -1)
            return -1 ;
        if(rows <= 0 || cols <= 0)
            return -1;

        term->rows = rows ;
        term->cols = cols;
        return 0;
    }
    term->rows = ws.ws_row;
    term->cols = ws.ws_col;
    return 0;
}