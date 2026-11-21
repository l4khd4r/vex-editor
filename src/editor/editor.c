#include "../../include/editor.h"
#include <string.h>
#include <unistd.h>
int editor_init(editor_t *ed, const char *filename) {

  memset(ed, 0, sizeof(editor_t));
  if (terminal_init(&ed->term) == -1)
    return -1;

  if (terminal_enable_raw_mode(&ed->term) == -1)
    return -1;

  if (terminal_get_size(&ed->term) == -1)
    return -1;

  if (filename)
    ed->doc = doc_open(filename);
  else
    ed->doc = doc_create();
  if (ed->doc == NULL) {
    terminal_destroy(&ed->term);
    return -1;
  }
  if (renderer_init(&ed->renderer, &ed->term) == -1) {
    terminal_destroy(&ed->term);
    doc_destroy(ed->doc);
    return -1;
  }
  ed->e_running = true;
  return 1;
}

void editor_destroy(editor_t *ed) {
  terminal_clear();
  doc_destroy(ed->doc);
  terminal_destroy(&ed->term);
}

void editor_process_key(editor_t *ed, int key) {

  switch (key) {
  case ARROW_UP:
    doc_move_up(ed->doc);
    break;
  case ARROW_RIGHT:
    doc_move_right(ed->doc);
    break;
  case ARROW_DOWN:
    doc_move_down(ed->doc);
    break;
  case ARROW_LEFT:
    doc_move_left(ed->doc);
    break;
  case END_KEY:
    doc_move_end(ed->doc);
    break;
  case HOME_KEY:
    doc_move_home(ed->doc);
    break;
  case CTRL_KEY('s'):
    doc_save(ed->doc);
    break;
  case CTRL_KEY('q'):
    ed->e_running = false;
    break;
  case CTRL_KEY('h'):
  case KEY_BACKSPACE:
    doc_backspace(ed->doc);
    break;
  case '\r':
    doc_insert_newline(ed->doc);
    break;
  case DEL_KEY:
    doc_delete(ed->doc);
    break;
  default:
    if ((key >= 32 && key <= 126) || key == '\t')
      doc_insert_char(ed->doc, key);
    break;
  }
}
void editor_run(editor_t *ed) {
  while (ed->e_running) {
    renderer_render(&ed->renderer, ed->doc);
    int key = terminal_read_key();
    if (key == -1)
      ed->e_running = false;
    else
      editor_process_key(ed, key);
  }
}
