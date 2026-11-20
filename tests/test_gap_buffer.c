#include "gap_buffer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* compare the whole logical text with an expected string */
static void expect_text(const gap_buffer_t *gb, const char *want) {
    size_t len = strlen(want);

    assert(gb_length(gb) == len);
    for (size_t i = 0; i < len; i++)
        assert(gb_char_at(gb, i) == (unsigned char)want[i]);
    assert(gb_char_at(gb, len) == -1);               /* past the end */
}

/* rules that must hold after every operation */
static void check_invariants(const gap_buffer_t *gb) {
    assert(gb->gap_start <= gb->gap_end);
    assert(gb->gap_end <= gb->capacity);
    assert(gb_cursor(gb) == gb->gap_start);
}

static void test_create(void) {
    gap_buffer_t *gb = gb_create(8);

    assert(gb != NULL);
    assert(gb_length(gb) == 0);
    assert(gb_cursor(gb) == 0);
    expect_text(gb, "");
    check_invariants(gb);
    gb_destroy(gb);

    gb_destroy(NULL);                                /* must not crash */
    printf("ok  create / destroy\n");
}

static void test_insert(void) {
    gap_buffer_t *gb = gb_create(8);

    assert(gb_insert(gb, 'h') == 0);
    assert(gb_insert(gb, 'i') == 0);
    expect_text(gb, "hi");
    assert(gb_cursor(gb) == 2);

    assert(gb_insert_string(gb, " there", 6) == 0);
    expect_text(gb, "hi there");
    assert(gb_insert_string(gb, "", 0) == 0);        /* empty is not an error */
    expect_text(gb, "hi there");
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  insert\n");
}

static void test_grow_keeps_right_side(void) {
    gap_buffer_t *gb = gb_create(4);

    gb_insert_string(gb, "abcd", 4);                 /* gap is now full */
    gb_move_left(gb);
    gb_move_left(gb);                                /* ab|cd */
    assert(gb_insert(gb, 'X') == 0);                 /* forces a grow */
    expect_text(gb, "abXcd");
    assert(gb_cursor(gb) == 3);
    assert(gb->capacity > 4);
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  grow keeps the text after the cursor\n");
}

static void test_create_zero(void) {
    gap_buffer_t *gb = gb_create(0);

    assert(gb != NULL);
    assert(gb_insert(gb, 'a') == 0);
    expect_text(gb, "a");
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  create(0)\n");
}

static void test_many_inserts(void) {
    gap_buffer_t *gb = gb_create(2);

    for (int i = 0; i < 1000; i++)
        assert(gb_insert(gb, (char)('a' + i % 26)) == 0);
    assert(gb_length(gb) == 1000);
    for (size_t i = 0; i < 1000; i++)
        assert(gb_char_at(gb, i) == 'a' + (int)(i % 26));
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  1000 inserts\n");
}

static void test_backspace_delete(void) {
    gap_buffer_t *gb = gb_create(8);

    gb_insert_string(gb, "abc", 3);
    assert(gb_backspace(gb) == 0);                   /* ab| */
    expect_text(gb, "ab");

    gb_move_to(gb, 0);                               /* |ab */
    assert(gb_backspace(gb) == -1);                  /* nothing on the left */
    assert(gb_delete(gb) == 0);                      /* |b */
    expect_text(gb, "b");

    gb_move_to(gb, 1);                               /* b| */
    assert(gb_delete(gb) == -1);                     /* nothing on the right */
    expect_text(gb, "b");
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  backspace / delete (and their edges)\n");
}

static void test_moves(void) {
    gap_buffer_t *gb = gb_create(8);

    gb_insert_string(gb, "hello", 5);
    assert(gb_move_right(gb) == -1);                 /* already at the end */
    assert(gb_move_left(gb) == 0);
    assert(gb_cursor(gb) == 4);
    assert(gb_move_right(gb) == 0);
    assert(gb_cursor(gb) == 5);

    assert(gb_move_to(gb, 0) == 0);
    assert(gb_cursor(gb) == 0);
    assert(gb_move_left(gb) == -1);                  /* already at the start */
    assert(gb_move_to(gb, 3) == 0);
    assert(gb_cursor(gb) == 3);
    assert(gb_move_to(gb, 5) == 0);                  /* end of text is valid */
    assert(gb_move_to(gb, 6) == -1);                 /* past the end */
    assert(gb_cursor(gb) == 5);                      /* unchanged */

    expect_text(gb, "hello");                        /* moving never changes text */
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  move left / right / to\n");
}

static void test_edit_in_middle(void) {
    gap_buffer_t *gb = gb_create(4);

    gb_insert_string(gb, "helo world", 10);
    gb_move_to(gb, 3);
    gb_insert(gb, 'l');                              /* fix the typo */
    expect_text(gb, "hello world");

    gb_move_to(gb, 5);
    gb_insert(gb, ',');
    expect_text(gb, "hello, world");

    gb_move_to(gb, 12);
    gb_insert(gb, '\n');                             /* newlines are just chars */
    expect_text(gb, "hello, world\n");
    check_invariants(gb);
    gb_destroy(gb);
    printf("ok  edits in the middle\n");
}

static void test_null_safety(void) {
    assert(gb_insert(NULL, 'a') == -1);
    assert(gb_insert_string(NULL, "a", 1) == -1);
    assert(gb_delete(NULL) == -1);
    assert(gb_backspace(NULL) == -1);
    assert(gb_move_left(NULL) == -1);
    assert(gb_move_right(NULL) == -1);
    assert(gb_move_to(NULL, 0) == -1);
    assert(gb_length(NULL) == 0);
    assert(gb_char_at(NULL, 0) == -1);
    printf("ok  NULL safety\n");
}

int main(void) {
    test_create();
    test_insert();
    test_grow_keeps_right_side();
    test_create_zero();
    test_many_inserts();
    test_backspace_delete();
    test_moves();
    test_edit_in_middle();
    test_null_safety();
    printf("\nall gap buffer tests passed\n");
    return 0;
}
