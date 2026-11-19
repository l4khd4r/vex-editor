#include "terminal.h"
#include <errno.h>
#include <unistd.h>




// try once: 1 = we got a char , 0 we got timeout and real errors for the shitt -1 
static int read_byte(unsigned char *out) {
    ssize_t n = read(STDIN_FILENO , out , 1 ) ;

    if (n == 1)
        return 1 ;
    if(n == -1 && errno != EINTR && errno != EAGAIN)
        return -1;
    return 0;
}

// called right after read the ESC 
static int decode_escape(void) { 
    unsigned char seq[3];
    if(read_byte(&seq[0]) != 1)
        return KEY_ESC;
    if(read_byte(&seq[1]) != 1)
        return KEY_ESC;
    
    if(seq[0] == '[') {
        // we get two possible things digit or alpha
        if(seq[1] >= '0' && seq[1] <= '9') {
            if(read_byte(&seq[2]) != 1 || seq[2] != '~')
                return KEY_ESC;
            
            switch(seq[1]) {
                case '1': case '7': return HOME_KEY;
                case '4': case '8': return END_KEY;
                case '3': return DEL_KEY;
                case '5': return PAGE_UP;
                case '6': return PAGE_DOWN;
            }
            return KEY_ESC;
        }
        switch(seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
        }
    }
    else if(seq[0] == 'O') {
        switch(seq[1]) {
            case 'A' : return ARROW_UP ;
            case 'B' : return ARROW_DOWN ;
            case 'C' : return  ARROW_RIGHT ;
            case 'D' : return ARROW_LEFT;
            case 'H' : return HOME_KEY ;
            case 'F' : return END_KEY ; 
        }
    }
    return KEY_ESC;
}


int terminal_read_key(void) {
    unsigned char c ;
    int r ;
    while((r = read_byte(&c)) == 0) ;
    if(r == -1)
        return -1 ;

    if(c == KEY_ESC)
        return decode_escape();
    return c ;
}
