#pragma once
#include <cstdint>

#include "board.hh"
#include "move.hh"

#ifndef KEY_ESC
#define KEY_ESC 0x1b
#endif

#ifndef DICE_NONE
#define DICE_NONE 0
#endif

#ifndef MAX_DICE
#define MAX_DICE 4
#endif

enum GameState {
  MAIN_MENU,
  PLAYER_TURN,
  MOVE,
  NO_MOVES,
  GAME_OVER,
  VISUALIZE_GAMEPLAY,
  QUIT
};

struct Game {
  GameState state;
  Board* board;
  uint8_t dice[MAX_DICE];
  uint8_t rolled[2];
  bool white_turn;
  MoveList* moves;
  int move_index;
  MoveList* history;
  char white_name[32];
  char red_name[32];
  Board** board_history;
};

Game* Game_new();
void Game_free(Game* self);
void Game_mainloop(Game* self);

void Game_prompt(Game* self);
void Game_handle_input(Game* g, GameState s, int c);

MoveList* MoveList_from_Game(Game* game);
