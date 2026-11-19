#ifndef VEX_H
#define VEX_H

#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>


typedef struct vex {
    terminal_t *term ;
    bool running ;
} vex_t ;



#endif