#include "../include/terminal.h"

static struct termios orig_termios;



void disable_raw_mode() {
    tcsetattr(STDIN_FILENO , TCSAFLUSH , &orig_termios);
}


void enable_raw_mode() {
    struct termios raw ;

    if(tcgetattr(STDIN_FILENO , &orig_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // register the disable_raw_mode function to be called at the exit of the program
    if(atexit(disable_raw_mode) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    // make a copy 
    raw = orig_termios ;
    // disable canonical mode and echoing 
    raw.c_lflag &= ~(ICANON | ECHO);
    // Disable special input processing
    raw.c_lflag &= ~(ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    // configure the read() behavior 

    raw.c_cc[VMIN] = 0; // minimum number of bytes to read
    raw.c_cc[VTIME] = 1; // timeout in tenths of a second
    // apply the new settings 

    if(tcsetattr(STDIN_FILENO , TCSAFLUSH , &raw))
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    
}