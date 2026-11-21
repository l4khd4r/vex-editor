#ifndef EDITOR_H
#define EDITOR_H
#include "document.h"
#include "renderer.h"
#include "terminal.h"
#include <stdbool.h>

#define CTRL_KEY(k) ((k) & 0x1f)
typedef struct {
  terminal_t term;
  document_t *doc;
  renderer_t renderer;
  bool e_running;
} editor_t;
int editor_init(editor_t *ed, const char *filename); /* filename may be NULL */
void editor_destroy(editor_t *ed);
void editor_process_key(editor_t *ed, int key);
void editor_run(editor_t *ed);
#endif
