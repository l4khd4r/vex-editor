#include "terminal.h"

#include <errno.h>

#include <unistd.h>

int terminal_write(const char *buffer, size_t len) {
    size_t written = 0;
    while(written < len ) {
        ssize_t n = write(STDOUT_FILENO , buffer + written , len - written );
        if(n == -1) {
            if(errno == EINTR)
                continue ;
            return -1 ;
        }
        written += (size_t)n ;

    }
    return 0;
}



int terminal_clear(void) {
    static const char seq[] = "\x1b[2J\x1b[H";
    return terminal_write(seq, sizeof(seq) -1 );
}