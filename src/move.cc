#include "move.hh"

#include <cstdlib>

#ifndef MOVELIST_DEFAULT_CAPACITY
#define MOVELIST_DEFAULT_CAPACITY 1
#endif

MoveList* MoveList_new() {
  MoveList* moves = (MoveList*)malloc(sizeof(MoveList));
  moves->size = 0;
  moves->capacity = 1;
  moves->data = (Move*)malloc(sizeof(Move) * moves->capacity);
  return moves;
}

void MoveList_free(MoveList* self) {
  if (self != NULL) {
    if (self->data != NULL) {
      free(self->data);
    }
    free(self);
  }
}

void MoveList_add(MoveList* self, uint8_t from, uint8_t to, int dice) {
  if (self->size == self->capacity) {
    self->capacity *= 2;
    self->data = (Move*)realloc(self->data, sizeof(Move) * self->capacity);
  }
  self->data[self->size].from = from;
  self->data[self->size].to = to;
  self->data[self->size].dice = dice;
  self->size++;
}

void MoveList_save(MoveList* self, FILE* f) {
  for (int i = 0; i < self->size; i++) {
    fprintf(f, "%d %d %d\n", self->data[i].from, self->data[i].to,
            self->data[i].dice);
  }
}