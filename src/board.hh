#pragma once
#include <cstdint>

struct Board {
  int8_t fields[24];
  int8_t bar[2];
};

Board* Board_new();
void Board_free(Board* self);
void Board_print(Board* board);
