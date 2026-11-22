#include "../../include/renderer.h"
#include "../../include/terminal.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void ab_init(append_buffer_t *ab) {
  ab->data = NULL;
  ab->cap = 0;
  ab->len = 0;
}

int ab_append(append_buffer_t *ab, const char *s, size_t n) {
  size_t need = ab->len + n;
  if (n == 0)
    return 0;
  if (need > ab->cap) { // need to reallocate in this case
    size_t new_cap = ab->cap ? ab->cap : 64;
    while (new_cap < need)
      new_cap *= 2;

    char *new_data = realloc(ab->data, new_cap);
    if (new_data == NULL)
      return -1;
    ab->data = new_data;
    ab->cap = new_cap;
  }
  memcpy(ab->data + ab->len, s, n);
  ab->len += n;
  return 0;
}

int ab_append_string(append_buffer_t *ab, const char *str) {
  return ab_append(ab, str, strlen(str));
}

void ab_free(append_buffer_t *ab) {
  if (ab->data) {
    free(ab->data);
    ab->data = NULL;
  }
  ab->cap = 0;
  ab->len = 0;
}

int renderer_init(renderer_t *r, terminal_t *term) {
  if (!r || !term || !term->initialized)
    return -1;
  if (term->rows < 3 || term->cols < 1)
    return -1; // we neeed + 1 for the bar + 1 for the text
  r->screen_cols = term->cols;
  r->screen_rows = term->rows - 2;
  r->row_offset = 0;
  r->col_offset = 0;
  return 0;
}

void renderer_scroll(renderer_t *r, const document_t *doc) {

  if (!doc || !r)
    return;
  size_t row = doc_cursor_row(doc);
  size_t col = doc_cursor_col(doc);

  if (row < r->row_offset)
    r->row_offset = row;
  if (row >= r->row_offset + r->screen_rows)
    r->row_offset = row - r->screen_rows + 1;

  if (col < r->col_offset)
    r->col_offset = col;
  if (col >= r->col_offset + r->screen_cols)
    r->col_offset = col - r->screen_cols + 1;
}

void renderer_draw_rows(const renderer_t *r, const document_t *doc,
                        append_buffer_t *ab) {
  if (!r || !doc || !ab)
    return;
  size_t count = doc_line_count(doc);
  for (size_t y = 0; y < r->screen_rows; y++) {
    size_t file_row = r->row_offset + y;
    if (file_row >= count)
      ab_append_string(ab, "~");
    else {
      size_t start = doc_line_start(doc, file_row);
      size_t length = doc_line_length(doc, file_row);
      size_t visible = 0;
      if (length > r->col_offset) {
        visible = length - r->col_offset;
        if (visible > r->screen_cols)
          visible = r->screen_cols;
      }
      for (size_t x = 0; x < visible; ++x) {
        int c = gb_char_at(doc->buffer, start + r->col_offset + x);
        if (c == '\t') {
          ab_append(ab, " ", 1);
        } else if (c < 32 || c == 127) {
          ab_append(ab, "?", 1);
        } else {
          char ch = (char)c;
          ab_append(ab, &ch, 1);
        }
      }
    }
    ab_append_string(ab, "\x1b[K");
    if (y < r->screen_rows - 1) {
      ab_append_string(ab, "\r\n");
    }
  }
}

void renderer_draw_status(renderer_t *r, const document_t *doc,
                          append_buffer_t *ab) {
  if (!r || !doc || !ab)
    return;
  ab_append_string(ab, "\r\n");
  ab_append_string(ab, "\x1b[7m");
  char left[256];
  const char *name = doc->filename ? doc->filename : "[No Name]";
  int n = snprintf(left, sizeof(left), " %s%s", name, doc->dirty ? " [+]" : "");

  if (n < 0)
    n = 0;
  if ((size_t)n >= sizeof(left))
    n = sizeof(left) - 1;
  size_t llen = (size_t)n;
  char right[64]; // here we can take only the shiity row and cols size

  size_t col = doc_cursor_col(doc);
  size_t row = doc_cursor_row(doc);
  n = snprintf(right, sizeof(right), "%zu:%zu ", row + 1, col + 1);
  if (n < 0)
    n = 0;
  if ((size_t)n >= sizeof(right))
    n = sizeof(right) - 1;

  size_t rlen = (size_t)n;
  if (llen > r->screen_cols)
    llen = r->screen_cols;
  ab_append(ab, left, llen);
  while (llen < r->screen_cols) {
    if (rlen == r->screen_cols - llen) {
      ab_append_string(ab, right);
      break;
    }
    ab_append(ab, " ", 1);
    llen++;
  }
  ab_append_string(ab, "\x1b[m");
}

void renderer_render(renderer_t *r, const document_t *doc, const char *message,
                     time_t message_time) {
  if (!r || !doc)
    return;
  renderer_scroll(r, doc);
  append_buffer_t ab;
  ab_init(&ab);

  ab_append_string(&ab, "\x1b[?25l");
  ab_append_string(&ab, "\x1b[H");

  renderer_draw_rows(r, doc, &ab);

  renderer_draw_status(r, doc, &ab);
  renderer_draw_message(r, message, message_time, &ab);
  size_t screen_row = doc_cursor_row(doc) - r->row_offset;
  size_t screen_col = doc_cursor_col(doc) - r->col_offset;

  char buf[32];
  int n = snprintf(buf, sizeof(buf), "\x1b[%zu;%zuH", screen_row + 1,
                   screen_col + 1);
  if (n < 0)
    n = 0;
  if ((size_t)n >= sizeof(buf))
    n = sizeof(buf) - 1;
  ab_append(&ab, buf, (size_t)n);
  ab_append_string(&ab, "\x1b[?25h"); // means show the cursor againa
  terminal_write(ab.data, ab.len);
  ab_free(&ab);
}

void renderer_draw_message(const renderer_t *r, const char *message,
                           time_t message_time, append_buffer_t *ab) {
  if (!r || !ab)
    return;
  ab_append_string(ab, "\r\n");       /* go to the last row */
  ab_append_string(ab, "\x1b[K");     /* clear it */
  if (!message || message[0] == '\0') /* no message */
    return;
  if (time(NULL) - message_time >= 5) /* older than 5 seconds */
    return;
  size_t len = strlen(message);
  if (len > r->screen_cols) /* cut to the screen width */
    len = r->screen_cols;
  ab_append(ab, message, len);
}
