#pragma once

#include <cstdint>

#ifndef MAX_MOVES
#define MAX_MOVES 48
#endif

#ifndef MOVE_FROM_WHITE_BAR
#define MOVE_FROM_WHITE_BAR 24
#endif

#ifndef MOVE_FROM_RED_BAR
#define MOVE_FROM_RED_BAR 25
#endif

#ifndef MOVE_TO_WHITE_COURT
#define MOVE_TO_WHITE_COURT 26
#endif

#ifndef MOVE_TO_RED_COURT
#define MOVE_TO_RED_COURT 27
#endif

struct Move {
  uint8_t from;
  uint8_t to;
  int dice;
};

struct MoveList {
  Move moves[MAX_MOVES];
  int size;
};

MoveList* MoveList_new();
void MoveList_free(MoveList* self);
void MoveList_add(MoveList* self, uint8_t from, uint8_t to, int dice);
