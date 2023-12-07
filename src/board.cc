#include "board.hh"

#include <ncurses.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "ui_utils.h"

Board* Board_new() {
  Board* self = (Board*)malloc(sizeof(Board));

  // clears all fields and properties of board
  memset(self, 0, sizeof(Board));

  self->fields[0] = -2;
  self->fields[5] = 5;
  self->fields[7] = 3;
  self->fields[11] = -5;
  self->fields[12] = 5;
  self->fields[16] = -3;
  self->fields[18] = -5;
  self->fields[23] = 2;
  return self;
}

void Board_free(Board* self) {
  if (self != NULL) {
    free(self);
  }
}

void print_pawn(int8_t pawn, bool highlighted) {
  cprintw(highlighted ? COLOR_YELLOW
          : pawn > 0  ? COLOR_WHITE
                      : COLOR_RED,
          pawn > 0 ? "  \u2B24 " : "  \u25EF ");
}

void print_default_field(int i, bool highlighted) {
  cprintw(highlighted ? COLOR_YELLOW : COLOR_BLUE,
          i & 1 ? "  \u0F0C " : "  \u0F1D ");
}

void print_pawn_or_default(Board* self, Move* move, int i, int j) {
  int8_t pawn = self->fields[abs(i)];

  if (pawn && abs(pawn) > abs(j)) {
    bool move_from_this_field = move != NULL && move->from == abs(i);
    print_pawn(pawn, move_from_this_field && abs(pawn) - 1 == abs(j));
  }

  else {
    print_default_field(i, move != NULL && move->to == abs(i));
  }
}

void print_fields_line(Board* self, Move* move, int i0, int i1, int j) {
  for (int i = i0; i < i1; i++) {
    print_pawn_or_default(self, move, i, j);
    if (i == i0 + 5) {
      // TODO: print pawns on bar
      cprintw(COLOR_BLUE, " | | ");
    }
  }
}

void print_fields(Board* self, Move* move, int i0, int i1, int j0, int j1) {
  for (int j = j0; j < j1; j++) {
    cprintw(COLOR_BLUE, "|");
    print_fields_line(self, move, i0, i1, j);
    cprintw(COLOR_BLUE, "|\n");
  }
}

void print_separator(const char* s0, const char* s1) {
  cprintw(COLOR_BLUE, "|");
  for (int i = 0; i < 12; i++) {
    cprintw(COLOR_BLUE, "%s", s0);
    if (i == 5) {
      cprintw(COLOR_BLUE, "%s", s1);
    }
  }
  cprintw(COLOR_BLUE, "|\n");
}

void print_field_numbers(int start, int end) {
  assert(end - start == 12);
  cprintw(COLOR_BLUE, "|");
  for (int i = start; i < end; i++) {
    cprintw(COLOR_BLUE, " %2d ", abs(i));
    if (i == start + 5) {
      printw("     ");
    }
  }
  cprintw(COLOR_BLUE, "|\n");
}

void Board_print(Board* self, Move* move) {
  print_separator("====", "=====");
  print_field_numbers(13, 25);
  print_separator("====", "=====");
  print_fields(self, move, 12, 24, 0, 6);
  print_separator("    ", " | | ");
  print_fields(self, move, -11, 1, -5, 1);
  print_separator("====", "=====");
  print_field_numbers(-12, 0);
  print_separator("====", "=====");
}

void Board_make_a_move(Board* self, bool white_turn, Move* move) {
  if (white_turn) {
    self->fields[move->from]--;
    self->fields[move->to]++;

    if (self->fields[move->to] == 0) {
      self->bar_red++;
      self->fields[move->to]++;
    }
  }

  else {
    self->fields[move->from]++;
    self->fields[move->to]--;

    if (self->fields[move->to] == 0) {
      self->bar_white++;
      self->fields[move->to]--;
    }
  }
}
