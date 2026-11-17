#include "../include/vex.h"
#include "../include/terminal.h"

int main() {

    vex_t *vex = malloc(sizeof(vex_t)) ;

    vex->buffer = NULL ;
    vex->bytes_read = 0 ;
    vex->buffer_size = 1024;
    vex->buffer= malloc(1024);
    enable_raw_mode(); // changing the terminal to raw mode 
    while(1) {
        vex->bytes_read = read(STDIN_FILENO , vex->buffer , 1024);
        if(vex->bytes_read == 1 && vex->buffer[0] == 'q') {
            free(vex->buffer);
            free(vex);
            char *message = "welcome anytime to my vex\n";
            write(STDOUT_FILENO , message , strlen(message));
            break ;
        }
        if(vex->bytes_read == 1 && vex->buffer[0] == '\r')
        {
            write(STDOUT_FILENO , "\r\n" , 2 );
            continue ;
        }

        // eather write the shit buffer 
        write(STDOUT_FILENO , vex->buffer , vex->bytes_read);

    }
    return  0 ;
}