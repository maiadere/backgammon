#include "board.hh"

#include <ncurses.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

Board* Board_new() {
  Board* self = (Board*)malloc(sizeof(Board));
  memset(self->fields, 0, sizeof(self->fields) / sizeof(self->fields[0]));
  self->fields[0] = 2;
  self->fields[5] = -5;
  self->fields[7] = -3;
  self->fields[11] = 5;
  self->fields[12] = -5;
  self->fields[16] = 3;
  self->fields[18] = 5;
  self->fields[23] = -2;
  self->bar[0] = self->bar[1] = 0;
  return self;
}

void Board_free(Board* self) { free(self); }

void print_fields(int8_t* fields, int i0, int i1, int j0, int j1) {
  assert(i1 - i0 == 12);
  assert(j1 - j0 == 6);
  for (int j = j0; j < j1; j++) {
    for (int i = i0; i < i1; i++) {
      int8_t x = fields[abs(i)];
      if (x && abs(x) > abs(j)) {
        addwstr(x > 0 ? L" \u25EF  " : L" \u2B24  ");
      } else {
        addwstr(i & 1 ? L"  \u0F0C " : L"  \u0F1D ");
      }
      if (i == i0 + 5) {
        // TODO: print pawns on bar
        printw(" | | ");
      }
    }
    addch(10);
  }
}

void print_separator(const char* s0, const char* s1) {
  for (int i = 0; i < 12; i++) {
    printw("%s", s0);
    if (i == 5) {
      printw("%s", s1);
    }
  }

  addch(10);
}

void print_field_numbers(int start, int end) {
  assert(end - start == 12);
  for (int i = start; i < end; i++) {
    printw(" %2d ", abs(i));
    if (i == start + 5) {
      printw("     ");
    }
  }

  addch(10);
}

void Board_print(Board* self) {
  print_field_numbers(13, 25);
  print_separator("====", "=====");
  print_fields(self->fields, 12, 24, 0, 6);
  print_separator("    ", " | | ");
  print_fields(self->fields, -11, 1, -5, 1);
  print_separator("====", "=====");
  print_field_numbers(-12, 0);
}
