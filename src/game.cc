#include "game.hh"

#include <ncurses.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "board.hh"
#include "move.hh"
#include "ui_utils.h"

Game* Game_new() {
  Game* self = (Game*)malloc(sizeof(Game));
  memset(self, 0, sizeof(Game));
  self->board = Board_new();
  self->state = MAIN_MENU;
  self->white_turn = rand() & 1;
  self->white_name[0] = self->red_name[0] = '\0';
  return self;
}

void Game_free(Game* self) {
  if (self != NULL) {
    Board_free(self->board);
    MoveList_free(self->moves);
    MoveList_free(self->history);
    if (self->board_history != NULL) {
      for (int i = 0; i < self->history->size + 1; i++) {
        Board_free(self->board_history[i]);
      }
      free(self->board_history);
    }
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

  return &self->moves->data[self->move_index];
}

bool Game_is_over(Game* self) {
  return self->board->court_white == 15 || self->board->court_red == 15;
}

bool Game_show_in_game_header(Game* self) {
  return self->state == PLAYER_TURN || self->state == MOVE ||
         self->state == NO_MOVES || self->state == GAME_OVER ||
         self->state == VISUALIZE_GAMEPLAY;
}

bool Game_hide_turn(Game* self) {
  return self->state == GAME_OVER || self->state == VISUALIZE_GAMEPLAY;
}

void Game_in_game_header(Game* self) {
  if (!Game_hide_turn(self)) {
    cprintw(COLOR_BLUE, "turn: ");
    cprintw(self->white_turn ? COLOR_WHITE : COLOR_RED,
            self->white_turn ? WHITE_PAWN_STR : RED_PAWN_STR);
  } else {
    nspaces(7);
  }
  cprintw(COLOR_BLUE, " |\n|%22s %s vs. %s %-22s|\n", self->white_name,
          WHITE_PAWN_STR, RED_PAWN_STR, self->red_name);
}

void Game_print_header(Game* self) {
  print_separator("====", "=====", "");
  cprintw(COLOR_BLUE, "| M. Stanka s197649");
  nspaces(27);
  if (Game_show_in_game_header(self)) {
    Game_in_game_header(self);
  } else {
    nspaces(8);
    cprintw(COLOR_BLUE, "|\n|");
    nspaces(53);
    cprintw(COLOR_BLUE, "|\n");
  }
}

void Game_mainloop(Game* self) {
  while (self->state != QUIT) {
    clear();
    Game_print_header(self);
    Board_print(self->board, Game_highlighted_move(self));
    addch('\n');
    Game_prompt(self);
    refresh();
    Game_handle_input(self, self->state, getch());
  }
}

void Game_print_moves(Game* self) {
  addch('\n');
  for (int i = 0; i < self->moves->size; i++) {
    Move* move = &self->moves->data[i];
    cprintw(i == self->move_index ? COLOR_YELLOW : COLOR_BLUE,
            "%d. %d -> %d        \n", i + 1, move->from + 1, move->to + 1);
  }
}

const char* Game_visualize_msg() {
  return "[Left] - previous move, [Right] - next move,\n"
         "[Up] - last move, [Down] - first move,\n"
         "[Enter] - Continue from current position, [ESC] - Exit to main menu";
}

const char* Game_get_prompt_msg(GameState s) {
  const char* m[] = {
      "[N] - New game, [V] - Visualize game, [ESC] - Exit",
      "[Enter] - Roll dice, [S] - Save to file, [ESC] - Exit",
      "[Arrow keys] - choose a move, [Enter] - Confirm",
      "No legal moves. [Enter] - Continue",
      "[S] - Save to file, [Enter] - Exit to main menu",
      Game_visualize_msg(),
  };
  return m[s];
}

void Game_game_over_msg(Game* self) {
  cprintw(COLOR_WHITE,
          self->board->court_white == 15 ? "White won! " : "Red won! ");
}

void Game_prompt(Game* self) {
  if (self->state == MOVE || self->state == NO_MOVES) {
    printw("Rolled: %d %d. ", self->rolled[0], self->rolled[1]);
  }

  if (self->state == MOVE) {
    Game_print_moves(self);
  }

  if (self->state == GAME_OVER) {
    Game_game_over_msg(self);
  }

  printw("%s", Game_get_prompt_msg(self->state));
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

void Game_save(Game* self, const char* filename) {
  FILE* f = fopen(filename, "w");
  if (f == NULL) {
    perror("fopen");
    return;
  }
  fprintf(f, "%s %s\n", self->white_name, self->red_name);
  MoveList_save(self->history, f);
  fclose(f);
}

void Game_save_input(Game* self) {
  static char filename[256] = {0};
  echo();
  cprintw(COLOR_WHITE, "\nEnter file name: ");
  refresh();
  scanw("%s", filename);
  noecho();
  Game_save(self, filename);
}

void Game_turn(Game* self, int c) {
  if (c == KEY_ESC) {
    self->state = MAIN_MENU;
  }

  if (c == '\n') {
    Game_dice_roll(self);

    self->moves = MoveList_from_Game(self);
    self->move_index = 0;

    self->state = self->moves->size == 0 ? NO_MOVES : MOVE;
  }

  if (c == 's') {
    Game_save_input(self);
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
  MoveList_add(self->history, move->from, move->to, move->dice);
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
    Game_make_a_move(self, &self->moves->data[self->move_index]);
    MoveList_free(self->moves);
    self->moves = NULL;

    if (Game_is_over(self)) {
      self->state = GAME_OVER;
      return;
    }

    if (Game_are_all_dice_none(self)) {
      self->state = PLAYER_TURN;
      self->white_turn = !self->white_turn;
    } else {
      Game_next_move(self);
    }
  }

  Game_navigate_moves(self, c);
}

void Game_prompt_player_names(Game* self) {
  echo();
  cprintw(COLOR_WHITE, "\nEnter white player name: ");
  refresh();
  scanw("%s", self->white_name);
  cprintw(COLOR_WHITE, "Enter red player name: ");
  refresh();
  scanw("%s", self->red_name);
  noecho();
}

void Game_load_input(Game* self) {
  static char filename[256] = {0};
  echo();
  cprintw(COLOR_WHITE, "\nEnter file name: ");
  refresh();
  scanw("%s", filename);
  noecho();

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    perror("fopen");
    return;
  }

  MoveList_free(self->history);
  self->history = MoveList_new();

  fscanf(f, "%s %s", self->white_name, self->red_name);
  while (!feof(f)) {
    int from, to, dice;
    fscanf(f, "%d %d %d", &from, &to, &dice);
    MoveList_add(self->history, from, to, dice);
  }

  fclose(f);
}

bool is_move_white_turn(Move move, Board* board) {
  return move.from < 24 ? board->fields[move.from] > 0
                        : (move.from == MOVE_FROM_WHITE_BAR ||
                           move.to == MOVE_TO_WHITE_COURT);
}

void init_boards_from_history(Game* self) {
  Board_free(self->board);
  self->board = Board_new();
  self->board_history =
      (Board**)malloc(sizeof(Board*) * (self->history->size + 1));
  self->board_history[0] = Board_new();
  self->move_index = 0;
}

void Game_boards_from_history(Game* self) {
  init_boards_from_history(self);

  for (int i = 0; i < self->history->size; i++) {
    self->board_history[i + 1] = (Board*)malloc(sizeof(Board));
    memcpy(self->board_history[i + 1], self->board_history[i], sizeof(Board));
    Move move = self->history->data[i];
    Board_make_a_move(self->board_history[i + 1],
                      is_move_white_turn(move, self->board_history[i]),
                      &self->history->data[i]);
  }
}

void Game_main_menu(Game* self, int c) {
  if (c == 'n') {
    MoveList_free(self->history);
    Board_free(self->board);
    self->board = Board_new();
    self->state = PLAYER_TURN;
    self->white_turn = rand() & 1;
    self->history = MoveList_new();
    Game_prompt_player_names(self);
  }

  if (c == 'v') {
    Game_load_input(self);
    Game_boards_from_history(self);
    memcpy(self->board, self->board_history[0], sizeof(Board));
    self->state = VISUALIZE_GAMEPLAY;
  }

  if (c == KEY_ESC) {
    self->state = QUIT;
  }
}

void Game_game_over_input(Game* self, int c) {
  if (c == '\n') {
    self->state = MAIN_MENU;
  }

  if (c == 's') {
    Game_save_input(self);
    self->state = MAIN_MENU;
  }
}

void visualize_prev_next_move(Game* self, int c) {
  if (c == KEY_LEFT) {
    self->move_index--;
    if (self->move_index < 0) {
      self->move_index = self->history->size - 1;
    }
  }

  if (c == KEY_RIGHT) {
    self->move_index++;
    if (self->move_index >= self->history->size) {
      self->move_index = 0;
    }
  }
}

void visualize_first_last_move(Game* self, int c) {
  if (c == KEY_UP) {
    self->move_index = self->history->size - 1;
  }

  if (c == KEY_DOWN) {
    self->move_index = 0;
  }
}

void visualize_continue(Game* self) {
  if (Game_is_over(self)) {
    self->state = GAME_OVER;
  } else {
    self->state = PLAYER_TURN;
    char c;
    echo();
    cprintw(COLOR_WHITE, "\nChoose turn (w/r): ");
    refresh();
    scanw("%c", &c);
    noecho();
    self->white_turn = !(c == 'r');
  }
}

void Game_visualize_gameplay(Game* self, int c) {
  if (c == KEY_ESC || c == '\n') {
    self->state = MAIN_MENU;
    for (int i = 0; i < self->history->size + 1; i++) {
      Board_free(self->board_history[i]);
    }
    free(self->board_history);
    self->board_history = NULL;
    if (c == '\n') {
      visualize_continue(self);
    }
  } else {
    visualize_prev_next_move(self, c);
    visualize_first_last_move(self, c);
    memcpy(self->board, self->board_history[self->move_index], sizeof(Board));
  }
}

void Game_handle_input(Game* g, GameState s, int c) {
  if (s == MAIN_MENU) Game_main_menu(g, c);
  if (s == PLAYER_TURN) Game_turn(g, c);
  if (s == NO_MOVES) Game_no_moves(g, c);
  if (s == MOVE) Game_move(g, c);
  if (s == GAME_OVER) Game_game_over_input(g, c);
  if (s == VISUALIZE_GAMEPLAY) Game_visualize_gameplay(g, c);
}
