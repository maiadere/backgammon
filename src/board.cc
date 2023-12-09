#include "board.hh"

#include <ncurses.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "move.hh"
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
  // self->fields[0] = 5;
  // self->fields[1] = 5;
  // self->fields[2] = 5;
  // self->fields[21] = -5;
  // self->fields[22] = -5;
  // self->fields[23] = -5;
  return self;
}

void Board_free(Board* self) {
  if (self != NULL) {
    free(self);
  }
}

#define WHITE_PAWN_STR "\u2B24"
#define RED_PAWN_STR "\u25EF"

void print_pawn(int8_t pawns, bool highlighted) {
  cprintw(highlighted ? COLOR_YELLOW
          : pawns > 0 ? COLOR_WHITE
                      : COLOR_RED,
          "  %s ", pawns > 0 ? WHITE_PAWN_STR : RED_PAWN_STR);
}

void print_default_field(int i, bool highlighted) {
  cprintw(highlighted ? COLOR_YELLOW : COLOR_BLUE,
          i & 1 ? "  \u1362 " : "  \u1368 ");
}

void print_pawn_or_default(Board* self, Move* move, int i, int j) {
  int8_t pawns = self->fields[abs(i)];

  if (pawns && abs(pawns) > abs(j)) {
    bool h =
        (move != NULL && move->from == abs(i) && abs(pawns) - 1 == abs(j)) ||
        (move != NULL && move->to == abs(i) && abs(pawns) - 1 == 5 &&
         abs(j) == 5);
    print_pawn(pawns, h);
  }

  else {
    if (move != NULL) {
      pawns = self->fields[move->to];
    }
    bool h = move != NULL && move->to == abs(i) && abs(pawns) == abs(j);
    print_default_field(i, h);
  }
}

void print_fields_line(Board* self, Move* move, int i0, int i1, int j) {
  for (int i = i0; i < i1; i++) {
    print_pawn_or_default(self, move, i, j);

    if (i == i0 + 5) {
      const char* s = " ";
      int bar_index = 5 - abs(j);

      if (j > 0 && bar_index < self->bar_white) {
        s = WHITE_PAWN_STR;
      } else if (j < 0 && bar_index < self->bar_red) {
        s = RED_PAWN_STR;
      }

      cprintw(COLOR_BLUE, " |%s| ", s);
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
    cprintw(COLOR_GREEN, " %2d ", abs(i));
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

void move_to_field_white(Board* self, Move* move) {
  self->fields[move->to]++;

  if (self->fields[move->to] == 0) {
    self->bar_red++;
    self->fields[move->to]++;
  }
}

void move_to_field_red(Board* self, Move* move) {
  self->fields[move->to]--;

  if (self->fields[move->to] == 0) {
    self->bar_white++;
    self->fields[move->to]--;
  }
}

void move_white(Board* self, Move* move) {
  if (move->from == MOVE_FROM_WHITE_BAR) {
    self->bar_white--;
  } else {
    self->fields[move->from]--;
  }

  if (move->to == MOVE_TO_WHITE_COURT) {
    self->court_white++;
  } else {
    move_to_field_white(self, move);
  }
}

void move_red(Board* self, Move* move) {
  if (move->from == MOVE_FROM_RED_BAR) {
    self->bar_red--;
  } else {
    self->fields[move->from]++;
  }

  if (move->to == MOVE_TO_RED_COURT) {
    self->court_red++;
  } else {
    move_to_field_red(self, move);
  }
}

void Board_make_a_move(Board* self, bool white_turn, Move* move) {
  if (white_turn) {
    move_white(self, move);
  } else {
    move_red(self, move);
  }
}
