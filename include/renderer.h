#ifndef RENDERER_H
#define RENDERER_H

#include "document.h"
#include "terminal.h"
#include <stddef.h>
// bufffer we need for the renderer
typedef struct {
  char *data;
  size_t len;
  size_t cap;
} append_buffer_t;

void ab_init(append_buffer_t *ab);
int ab_append(append_buffer_t *ab, const char *s,
              size_t n); // append only n bytes of the s
int ab_append_string(append_buffer_t *ab,
                     const char *str); // append the string str into the ab

void ab_free(append_buffer_t *ab);

// the struct of the renderer (display)
typedef struct {
  size_t screen_rows;
  size_t screen_cols;
  size_t row_offset;
  size_t col_offset;
} renderer_t;

int renderer_init(renderer_t *r, terminal_t *term);
void renderer_scroll(renderer_t *r, const document_t *doc);
void renderer_draw_rows(const renderer_t *r, const document_t *doc,
                        append_buffer_t *ab);

void renderer_draw_status(renderer_t *r, const document_t *doc,
                          append_buffer_t *ab);
void renderer_render(renderer_t *r, const document_t *doc);
#endif
