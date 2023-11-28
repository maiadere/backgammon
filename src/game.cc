#include "game.hh"

#include <ncurses.h>

#include <cstdlib>

Game* Game_new() {
  Game* self = (Game*)malloc(sizeof(Game));
  self->board = Board_new();
  self->state = WAITING_FOR_ROLL;
  self->dices[0] = self->dices[1] = 0;
  self->turn = 0;
  return self;
}

void Game_free(Game* self) {
  Board_free(self->board);

  if (self != NULL) {
    free(self);
  }
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
  printw("\nCurrent turn: ");
  addwstr(self->turn == 0 ? L"\u2B24\n" : L"\u25EF\n");
  switch (self->state) {
    case WAITING_FOR_ROLL:
      printw("Press any key to roll dice...");
      break;
    case ROLLED:
      printw("You rolled: %d %d\n", self->dices[0], self->dices[1]);
      printw("Press any key...");
      break;
    default:
      break;
  }
}

void Game_handle_input(Game* self, int c) {
  if (c == KEY_ESC) {
    self->state = QUIT;
  }

  if (self->state == WAITING_FOR_ROLL) {
    self->dices[0] = 1 + rand() % 6;
    self->dices[1] = 1 + rand() % 6;
    self->state = ROLLED;
  } else if (self->state == ROLLED) {
    self->state = WAITING_FOR_ROLL;
    self->turn = self->turn == 0 ? 1 : 0;
  }
}
