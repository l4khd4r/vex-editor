#include "../../include/gap_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* lifecycle */
gap_buffer_t *gb_create(size_t initial_capacity) {
  gap_buffer_t *gb = NULL;
  size_t cap = initial_capacity ?: 16;
  gb = calloc(1, sizeof(gap_buffer_t));
  if (!gb)
    return NULL;
  gb->buffer = calloc(cap, 1);
  if (!gb->buffer) {
    free(gb);
    gb = NULL;
    return NULL;
  }
  gb->capacity = cap;
  gb->gap_start = 0;
  gb->gap_end = cap;
  return gb;
}

void gb_destroy(gap_buffer_t *gb) {
  if (!gb)
    return;
  if (gb->buffer) {
    free(gb->buffer);
    gb->buffer = NULL;
  }

  free(gb);
}

/*edits are done implementating*/
static int gb_grow(gap_buffer_t *gb) {
  size_t new_capacity = gb->capacity ? gb->capacity * 2 : 16;
  char *new_buffer = calloc(new_capacity, 1);
  if (!new_buffer)
    return -1;

  size_t right_len = gb->capacity - gb->gap_end;
  memcpy(new_buffer, gb->buffer, gb->gap_start);
  memcpy(new_buffer + new_capacity - right_len, gb->buffer + gb->gap_end,
         right_len);
  free(gb->buffer);
  gb->buffer = new_buffer;
  gb->gap_end = new_capacity - right_len;
  gb->capacity = new_capacity;
  return 0;
}
int gb_insert(gap_buffer_t *gb, char c) {
  if (!gb || !gb->buffer)
    return -1;

  if (gb->gap_start >= gb->gap_end) {
    // need to grow.
    if (gb_grow(gb) == -1)
      return -1;
  }
  gb->buffer[gb->gap_start] = c;
  gb->gap_start++;
  return 0;
}

int gb_insert_string(gap_buffer_t *gb, const char *str, size_t len) {
  if (!gb || !gb->buffer || !str)
    return -1;
  if (len == 0)
    return 0;

  for (size_t i = 0; i < len; i++) {
    if (gb_insert(gb, str[i]) == -1) {
      return -1;
    }
  }
  return 0;
}

int gb_delete(gap_buffer_t *gb) {
  if (!gb || !gb->buffer)
    return -1;

  if (gb->gap_end >= gb->capacity)
    return -1;
  gb->gap_end++;
  return 0;
}

int gb_backspace(gap_buffer_t *gb) {
  if (!gb || !gb->buffer)
    return -1;

  if (gb->gap_start == 0)
    return -1;

  gb->gap_start--;
  return 0;
}

/*movements now */

int gb_move_left(gap_buffer_t *gb) {
  if (!gb || !gb->buffer)
    return -1;

  if (gb->gap_start == 0)
    return -1;

  // we shift the shit to left
  gb->gap_end--;
  gb->buffer[gb->gap_end] = gb->buffer[gb->gap_start - 1];
  gb->gap_start--;
  return 0;
}

int gb_move_right(gap_buffer_t *gb) {
  if (!gb || !gb->buffer)
    return -1;

  if (gb->gap_end >= gb->capacity)
    return -1;
  // we shift the shit to right
  gb->buffer[gb->gap_start] = gb->buffer[gb->gap_end];
  gb->gap_start++;
  gb->gap_end++;
  return 0;
}

int gb_move_to(gap_buffer_t *gb, size_t pos) {
  if (!gb || !gb->buffer)
    return -1;

  if (pos > gb_length(gb))
    return -1;
  while (gb->gap_start != pos) {
    if (gb->gap_start < pos) {
      if (gb_move_right(gb) == -1)
        return -1;
    } else {
      if (gb_move_left(gb) == -1)
        return -1;
    }
  }
  return 0;
}

/* queries (read-only) */

size_t gb_length(const gap_buffer_t *gb) {
  if (!gb)
    return 0;
  return gb->capacity - (gb->gap_end - gb->gap_start);
}

size_t gb_cursor(const gap_buffer_t *gb) {
  if (!gb)
    return 0;
  return gb->gap_start;
}

int gb_char_at(const gap_buffer_t *gb, size_t pos) {
  if (!gb || !gb->buffer || pos >= gb_length(gb))
    return -1;

  if (pos < gb->gap_start)
    return (unsigned char)gb->buffer[pos];
  return (unsigned char)gb->buffer[pos + (gb->gap_end - gb->gap_start)];
}

/*debugger*/

void gb_debug_print(const gap_buffer_t *gb) {
  if (!gb || !gb->buffer)
    return;

  for (size_t i = 0; i <= gb->capacity; i++) {
    if (i == gb->gap_start)
      putchar('[');
    if (i == gb->gap_end)
      putchar(']');
    if (i == gb->capacity)
      break; /* only for the brackets */

    if (i >= gb->gap_start && i < gb->gap_end) {
      putchar('_');
      continue;
    }
    unsigned char c = gb->buffer[i];
    if (c == '\n')
      printf("\\n");
    else if (c == '\t')
      printf("\\t");
    else if (c < 32 || c > 126)
      putchar('?');
    else
      putchar(c);
  }
  printf("  gs=%zu ge=%zu cap=%zu len=%zu\n", gb->gap_start, gb->gap_end,
         gb->capacity, gb_length(gb));
}
