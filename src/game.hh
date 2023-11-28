#pragma once
#include "board.hh"

#ifndef KEY_ESC
#define KEY_ESC 0x1b
#endif

enum GameState { WAITING_FOR_ROLL, ROLLED, QUIT };

struct Game {
  GameState state;
  Board* board;
  uint8_t dices[2];
  uint8_t turn;
};

Game* Game_new();
void Game_free(Game* self);
void Game_mainloop(Game* self);

void Game_prompt(Game* self);
void Game_handle_input(Game* self, int c);
