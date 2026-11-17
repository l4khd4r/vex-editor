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
    int cursor_x ;
    int cursor_y ;
};
typedef struct vex vex_t ;

typedef struct key {
    char c ;
    int type ;
    char arrow ;
} vex_key_t ;




#endif