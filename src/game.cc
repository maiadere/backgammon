#include "game.hh"

#include <ncurses.h>

#include <cstdlib>

Game* Game_new() {
  Game* self = (Game*)malloc(sizeof(Game));
  self->board = Board_new();
  self->state = WAITING_FOR_ROLL;
  self->dices[0] = self->dices[1] = 0;
  return self;
}

void Game_free(Game* self) {
  Board_free(self->board);
  free(self);
}

void Game_mainloop(Game* self) {
  while (self->state != QUIT) {
    clear();
    Board_print(self->board);
    Game_prompt(self);
    refresh();
    Game_handle_input(self, getch());
  }
}

void Game_prompt(Game* self) {
  switch (self->state) {
    case WAITING_FOR_ROLL:
      printw("\nPress enter to roll dice...");
      break;
    case ROLLED:
      printw("\nRolled: %d %d", self->dices[0], self->dices[1]);
      printw("\nPress any key...");
      break;
    default:
      break;
  }
}

void Game_handle_input(Game* self, int c) {
  if (self->state == WAITING_FOR_ROLL && c == '\n') {
    self->dices[0] = 1 + rand() % 6;
    self->dices[1] = 1 + rand() % 6;
    self->state = ROLLED;
  } else {
    self->state = WAITING_FOR_ROLL;
    if (c == 'q') {
      self->state = QUIT;
    }
  }
}
