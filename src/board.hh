#pragma once
#include <cstdint>

#include "move.hh"

struct Board {
  /// - 0: empty field
  /// - positive values: number of white pawns
  /// - negative values: number of red pawns
  int8_t fields[24];

  int8_t bar_white, bar_red;
  int8_t court_white, court_red;
};

Board* Board_new();
void Board_free(Board* self);
void Board_print(Board* board, Move* move);

void Board_make_a_move(Board* self, bool white_turn, Move* move);
