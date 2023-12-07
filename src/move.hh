#pragma once

#include <cstdint>

#ifndef MAX_MOVES
#define MAX_MOVES 48
#endif

struct Move {
  uint8_t from;
  uint8_t to;
};

struct MoveList {
  Move moves[MAX_MOVES];
  int size;
};

MoveList* MoveList_new();
void MoveList_free(MoveList* self);
void MoveList_add(MoveList* self, uint8_t from, uint8_t to);
