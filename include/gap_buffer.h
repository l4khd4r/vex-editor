#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H
#include <stddef.h>

typedef struct {
    char    *buffer ; // text + gap  , all of the physical array
    size_t  capacity ; // total size of the buf 
    size_t  gap_start; // first empty slot = cursor ( logical pos)
    size_t  gap_end;    // fist slot after the gap 
}   gap_buffer_t ;


/*lifecycle*/
gap_buffer_t    *gb_create(size_t initial_capacity) ;
void            gb_destroy(gap_buffer_t *gb) ;

/*edit*/
int             gb_insert(gap_buffer_t *gb , char c); 
int             gb_insert_string(gap_buffer_t *gb , const char *str , size_t len);
int             gb_delete(gap_buffer_t *gb) ;
int             gb_backspace(gap_buffer_t *gb) ;

/*cursor movment*/
int gb_move_left(gap_buffer_t *gb);
int gb_move_right(gap_buffer_t *gb);
int gb_move_to(gap_buffer_t *gb, size_t pos);


/* queries (read-only) */

size_t gb_length(const gap_buffer_t *gb);
size_t gb_cursor(const gap_buffer_t *gb) ;
int    gb_char_at(const gap_buffer_t *gb , size_t pos) ;


/*debugger*/
void gb_debug_print(const gap_buffer_t *gb);

#endif