#include "../include/vex.h"
#include "../include/terminal.h"
#include "../include/utils.h"
bool editor_running = true ;

int main() {

    vex_t *editor = calloc(1 , sizeof(vex_t)) ;

    editor->buffer = NULL ;
    editor->bytes_read = 0 ;
    editor->buffer_size = 1024;
    editor->buffer= malloc(1024);
    
    enable_raw_mode(); // changing the terminal to raw mode 
    while(editor_running) {
        vex_key_t key = read_key() ;
        switch (key.type ){
            case KEY_CHARACTER:
                insert_char(editor, key.c) ;
                break ;

            case KEY_ENTER:
                insert_newline(editor);
                break ;

            case KEY_ARROW:
                move_cursor(editor  , key.arrow) ;
                break ;

            case KEY_QUIT:
                editor_running = false ;
                break ;
            
            case KEY_BACKSPACE:
                delete_char(editor);
                break;
            case KEY_SAVE:
                save_file(editor);
                break;
        }
        refresh_screen(editor);
    }
    // at the exit we disable so no need callined the disable_raw_mode() function
    free(editor->buffer) ;
    free(editor);
    
    return  0 ;
}