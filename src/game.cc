#include "game.hh"

#include <ncurses.h>

#include <cstdlib>
#include <cstring>

#include "move.hh"
#include "ui_utils.h"

Game* Game_new() {
  Game* self = (Game*)malloc(sizeof(Game));
  memset(self, 0, sizeof(Game));
  self->board = Board_new();
  self->state = PLAYER_TURN;
  self->white_turn = rand() & 1;
  return self;
}

void Game_free(Game* self) {
  Board_free(self->board);

  if (self != NULL) {
    free(self);
  }
}

Move* Game_highlighted_move(Game* self) {
  if (self->moves == NULL) {
    return NULL;
  }

  if (self->move_index < 0 && self->move_index >= self->moves->size) {
    return NULL;
  }

  return &self->moves->moves[self->move_index];
}

void Game_mainloop(Game* self) {
  while (self->state != QUIT) {
    clear();
    cprintw(COLOR_BLUE, "|                                             turn: ");
    cprintw(self->white_turn ? COLOR_WHITE : COLOR_RED,
            self->white_turn ? "\u2B24" : "\u25EF");
    cprintw(COLOR_BLUE, " |\n");
    Board_print(self->board, Game_highlighted_move(self));
    addch('\n');
    Game_prompt(self);
    refresh();
    Game_handle_input(self, self->state, getch());
  }
}

void Game_print_moves(Game* self) {
  printw("dice: %d %d %d %d\n", self->dice[0], self->dice[1], self->dice[2],
         self->dice[3]);
  addch('\n');
  for (int i = 0; i < self->moves->size; i++) {
    Move* move = &self->moves->moves[i];
    cprintw(i == self->move_index ? COLOR_YELLOW : COLOR_BLUE,
            "%d. %d -> %d        \n", i + 1, move->from + 1, move->to + 1);
  }
}

void Game_prompt(Game* self) {
  const char* msgs[] = {"[Enter] - Roll dice, [ESC] - Exit",
                        "[Arrow keys] - choose a move, [Enter] - Confirm",
                        "No legal moves. Press [Enter] to continue..."};

  if (self->state == MOVE || self->state == NO_MOVES) {
    printw("Rolled: %d %d. ", self->rolled[0], self->rolled[1]);
  }

  if (self->state == MOVE) {
    Game_print_moves(self);
  }

  printw("%s", msgs[self->state]);
}

void Game_dice_roll(Game* self) {
  self->dice[0] = 1 + rand() % 6;
  self->dice[1] = 1 + rand() % 6;

  if (self->dice[0] == self->dice[1]) {
    self->dice[2] = self->dice[0];
    self->dice[3] = self->dice[0];
  } else {
    self->dice[2] = DICE_NONE;
    self->dice[3] = DICE_NONE;
  }

  self->rolled[0] = self->dice[0];
  self->rolled[1] = self->dice[1];
}

void Game_turn(Game* self, int c) {
  if (c == KEY_ESC) {
    self->state = QUIT;
  }

  if (c == '\n') {
    Game_dice_roll(self);

    self->moves = MoveList_from_Game(self);
    self->move_index = 0;

    self->state = self->moves->size == 0 ? NO_MOVES : MOVE;
  }
}

void Game_no_moves(Game* self, int c) {
  if (c == '\n') {
    self->state = PLAYER_TURN;
    self->white_turn = !self->white_turn;

    MoveList_free(self->moves);
    self->moves = NULL;
  }
}

void Game_make_a_move(Game* self, Move* move) {
  Board_make_a_move(self->board, self->white_turn, move);

  for (int i = 0; i < MAX_DICE; i++) {
    if (self->dice[i] == move->dice) {
      self->dice[i] = DICE_NONE;
      break;
    }
  }
}

void Game_navigate_moves(Game* self, int c) {
  if (c == KEY_LEFT) {
    self->move_index--;
    if (self->move_index < 0) {
      self->move_index = self->moves->size - 1;
    }
  }

  else if (c == KEY_RIGHT) {
    self->move_index++;
    if (self->move_index >= self->moves->size) {
      self->move_index = 0;
    }
  }
}

void Game_next_move(Game* self) {
  self->moves = MoveList_from_Game(self);
  self->move_index = 0;

  if (self->moves->size == 0) {
    MoveList_free(self->moves);
    self->moves = NULL;
    self->state = PLAYER_TURN;
    self->white_turn = !self->white_turn;
  }
}

bool Game_are_all_dice_none(Game* self) {
  for (int i = 0; i < MAX_DICE; i++) {
    if (self->dice[i] != DICE_NONE) {
      return false;
    }
  }
  return true;
}

void Game_move(Game* self, int c) {
  if (c == '\n') {
    Game_make_a_move(self, &self->moves->moves[self->move_index]);
    MoveList_free(self->moves);
    self->moves = NULL;

    if (Game_are_all_dice_none(self)) {
      self->state = PLAYER_TURN;
      self->white_turn = !self->white_turn;
    } else {
      Game_next_move(self);
    }
  }

  Game_navigate_moves(self, c);
}

void Game_handle_input(Game* g, GameState s, int c) {
  if (s == PLAYER_TURN)
    Game_turn(g, c);

  else if (s == NO_MOVES)
    Game_no_moves(g, c);

  else if (s == MOVE)
    Game_move(g, c);
}
