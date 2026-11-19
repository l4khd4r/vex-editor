#include "terminal.h"
#include <errno.h>
#include <unistd.h>


int terminal_read_key(void) {
    unsigned char c ;
    ssize_t  n ;

    while((n = read(STDIN_FILENO ,  &c , 1 )) != 1 ) {
        if(n == -1 && errno != EINTR)
            return -1 ;
    }
    return c ;
}