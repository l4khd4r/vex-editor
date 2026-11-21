#include "../include/document.h"
#include "../include/renderer.h"
#include "../include/terminal.h"
#define CTRL_KEY(k) ((k) & 0x1f)
int main(int argc, char **argv) {
  // first step related to changing in the terminla setup
  terminal_t term = {0};

  if (terminal_init(&term) == -1)
    return 1;

  if (terminal_enable_raw_mode(&term) == -1) {
    return 1;
  }
  if (terminal_get_size(&term) == -1) {
    terminal_destroy(&term);
    return 1;
  }

  // step 2 : open the file and read it in a doc

  document_t *doc;
  if (argc >= 2)
    doc = doc_open(argv[1]);
  else
    doc = doc_create();
  if (doc == NULL) {
    terminal_destroy(&term);
    return 1;
  }

  renderer_t r;
  if (renderer_init(&r, &term) == -1) {
    terminal_destroy(&term);
    doc_destroy(doc);
    return 1;
  }

  for (;;) {
    renderer_render(&r, doc);
    int c = terminal_read_key();
    if (c == -1 || c == CTRL_KEY('q'))
      break;

    switch (c) {
    case ARROW_UP:
      doc_move_up(doc);
      break;
    case ARROW_RIGHT:
      doc_move_right(doc);
      break;
    case ARROW_DOWN:
      doc_move_down(doc);
      break;
    case ARROW_LEFT:
      doc_move_left(doc);
      break;
    case END_KEY:
      doc_move_end(doc);
      break;
    case HOME_KEY:
      doc_move_home(doc);
      break;
    }
  }

  terminal_clear();
  doc_destroy(doc);
  terminal_destroy(&term);
  return 0;
}
