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
