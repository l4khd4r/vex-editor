#include "../../include/document.h"
#include <stdio.h>
#include <stdlib.h>

#include "../../include/gap_buffer.h"
document_t *doc_create(void) {
  document_t *doc = calloc(1, sizeof(document_t));
  if (!doc)
    return NULL;
  doc->buffer = gb_create(16);
  if (!doc->buffer) {
    free(doc);
    return NULL;
  }
  doc->filename = NULL;
  doc->dirty = false;
  doc->preferred_col = 0;
  return doc;
}

void doc_destroy(document_t *doc) {
  if (!doc)
    return;
  gb_destroy(doc->buffer);
  free(doc->filename);
  free(doc);
}

// document_t *doc_open(const char *path) {
//     if(!path)
//         return NULL ;
//     FILE *fp = fopen(path , "r");
//     if(!fp)
//         return NULL;

//     document_t *doc = doc_create();
//     if(!doc) {
//         fclose(fp);
//         return NULL;
//     }
//     fseek(fp , 0 , SEEK_END);
// }

int doc_insert_char(document_t *doc, char c) {
  if (!doc || !doc->buffer)
    return -1;

  if (gb_insert(doc->buffer, c) == -1)
    return -1;
  doc->dirty = true;
  return 0;
}

int doc_insert_newline(document_t *doc) {
  if (!doc || !doc->buffer)
    return -1;

  if (gb_insert(doc->buffer, '\n') == -1)
    return -1;
  doc->dirty = true;
  return 0;
}

int doc_backspace(document_t *doc) {
  if (!doc || !doc->buffer)
    return -1;

  if (gb_backspace(doc->buffer) == -1)
    return -1;
  doc->dirty = true;
  return 0;
}

int doc_delete(document_t *doc) {
  if (!doc || !doc->buffer)
    return -1;

  if (gb_delete(doc->buffer) == -1)
    return -1;
  doc->dirty = true;
  return 0;
}

size_t doc_line_count(const document_t *doc) {
  if (!doc || !doc->buffer)
    return 0;

  size_t count = 0;
  size_t len = gb_length(doc->buffer);
  for (size_t i = 0; i < len; i++) {
    if (gb_char_at(doc->buffer, i) == '\n')
      count++;
  }
  return count + 1;
}

size_t doc_line_start(const document_t *doc, size_t row) {

  if (row == 0)
    return 0;
  gap_buffer_t buf = {0};
  if (!doc || !doc->buffer)
    return 0;
  buf = *doc->buffer;
  size_t length = gb_length(&buf);

  size_t seen = 0;
  size_t i = 0;
  for (; i < length; i++) {
    if (gb_char_at(&buf, i) == '\n')
      seen++;
    if (seen == row)
      return i + 1;
  }
  return length;
}

size_t doc_line_length(const document_t *doc, size_t row) {
  if (!doc || !doc->buffer)
    return 0;

  size_t length = gb_length(doc->buffer);
  size_t i = doc_line_start(doc, row);

  size_t n = 0;
  for (; i < length && gb_char_at(doc->buffer, i) != '\n'; i++, n++)
    ;
  return n;
}
