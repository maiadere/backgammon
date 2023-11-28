#include "board.hh"

#include <ncurses.h>

#include <cstdlib>
#include <cstring>

Board* Board_new() {
  Board* self = (Board*)malloc(sizeof(Board));
  memset(self->fields, 0, sizeof(self->fields));
  self->fields[0] = 2;
  self->fields[5] = -5;
  self->fields[7] = -3;
  self->fields[11] = 5;
  self->fields[12] = -5;
  self->fields[16] = 3;
  self->fields[18] = 5;
  self->fields[23] = -2;
  return self;
}

void print_fields(int8_t* fields, int i0, int i1, int j0, int j1) {
  for (int j = j0; j < j1; j++) {
    for (int i = i0; i < i1; i++) {
      int8_t x = fields[abs(i)];
      if (x && abs(x) > abs(j)) {
        addwstr(x > 0 ? L" \u25EF  " : L" \u2B24  ");
      } else {
        printw(" .. ");
      }
      if (i == i0 + 5) {
        printw(" |  | ");
      }
    }
    addch(10);
  }
}

void print_separator() {
  for (int i = 0; i < 12; i++) {
    printw("====");
    if (i == 5) {
      printw(" |  | ");
    }
  }

  addch(10);
}

void print_field_numbers(int start, int end) {
  for (int i = start; i < end; i++) {
    printw(" %2d ", abs(i));
    if (i == (start + end) / 2 - 1) {
      printw(" |  | ");
    }
  }

  addch(10);
}

void Board_print(Board* board) {
  print_field_numbers(13, 25);
  print_separator();
  print_fields(board->fields, 12, 24, 0, 6);
  print_separator();
  print_fields(board->fields, -11, 1, -5, 1);
  print_separator();
  print_field_numbers(-12, 0);
}
