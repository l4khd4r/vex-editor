#include "../include/editor.h"

int main(int argc, char **argv) {
  editor_t ed;

  if (editor_init(&ed, argc >= 2 ? argv[1] : NULL) == -1)
    return 1;
  editor_run(&ed);
  editor_destroy(&ed);
  return 0;
}
