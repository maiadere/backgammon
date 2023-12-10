#pragma once

#include <cstdint>
#include <cstdio>

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
  Move* data;
  int size;
  int capacity;
};

MoveList* MoveList_new();
void MoveList_free(MoveList* self);
void MoveList_add(MoveList* self, uint8_t from, uint8_t to, int dice);
void MoveList_save(MoveList* self, FILE* f);
