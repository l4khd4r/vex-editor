#ifndef VEX_H
#define VEX_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>



struct vex {
    char *buffer; 
    int bytes_read ;
    int buffer_size ;
};

typedef struct vex vex_t ;

#endif