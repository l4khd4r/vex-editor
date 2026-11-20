#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <stddef.h>
#include <stdbool.h>
#include "gap_buffer.h"
typedef struct  {
    char *filename ;
    gap_buffer_t *buffer ;
    bool dirty ;
    size_t preferred_col ; /*sticky column for up/down*/
}document_t ;

/* lifecycle */

document_t *doc_create(void);
document_t *doc_open(const char *path);

void doc_destroy(document_t *doc) ;
int doc_save(document_t *doc);

/*editing*/
int doc_insert_char(document_t *doc , char c );
int doc_insert_newline(document_t *doc);
int doc_backspace(document_t *doc);
int doc_delete(document_t *doc);

/* lines(read-only)*/

size_t doc_line_count(const document_t *doc);
size_t doc_line_start(const document_t *doc, size_t row);
size_t doc_line_length(const document_t *doc , size_t row);

/*cursor position*/
size_t doc_cursor_row(const document_t *doc);
size_t doc_cursor_col(const document_t *doc);
int doc_move_to(document_t *doc , size_t row , size_t col);

/* cursor movment for arrow keys*/

int doc_move_left(document_t *doc);
int doc_move_right(document_t *doc);
int doc_move_down(document_t *doc);
int doc_move_up(document_t *doc);
int doc_move_end(document_t *doc);
int doc_move_home(document_t *doc);


#endif