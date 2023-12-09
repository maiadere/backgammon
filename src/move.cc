#include "move.hh"

#include <cassert>
#include <cstdlib>

MoveList* MoveList_new() {
  MoveList* moves = (MoveList*)malloc(sizeof(MoveList));
  moves->size = 0;
  return moves;
}

void MoveList_free(MoveList* self) {
  if (self != NULL) {
    free(self);
  }
}

void MoveList_add(MoveList* self, uint8_t from, uint8_t to, int dice) {
  assert(self->size < MAX_MOVES);
  self->moves[self->size].from = from;
  self->moves[self->size].to = to;
  self->moves[self->size].dice = dice;
  self->size++;
}
