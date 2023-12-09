#include <ncurses.h>

#include "game.hh"
#include "move.hh"

bool can_move(Game* game, int f, int d) {
  if (f + d < 0 || f + d > 23) {
    return false;
  }

  int field = game->board->fields[f + d];

  if (!game->white_turn) {
    field = -field;
  }

  return field >= -1;
}

void check_field_and_dice(Game* game, MoveList* moves, int field, int dice) {
  if (dice == DICE_NONE) {
    return;
  }

  if (game->white_turn && game->board->fields[field] > 0 &&
      can_move(game, field, -dice)) {
    MoveList_add(moves, field, field - dice, dice);
  }

  if (!game->white_turn && game->board->fields[field] < 0 &&
      can_move(game, field, dice)) {
    MoveList_add(moves, field, field + dice, dice);
  }
}

void check_field(Game* game, MoveList* moves, int field) {
  if (game->white_turn && game->board->bar_white > 0) {
    return;
  }

  if (!game->white_turn && game->board->bar_red > 0) {
    return;
  }

  for (int i = 0; i < MAX_DICE; i++) {
    check_field_and_dice(game, moves, field, game->dice[i]);
  }
}

void check_moves_from_bar(Game* game, MoveList* moves, int dice) {
  if (dice == DICE_NONE) {
    return;
  }

  if (game->white_turn && game->board->bar_white > 0) {
    int move_to = 24 - dice;

    if (game->board->fields[move_to] >= -1) {
      MoveList_add(moves, MOVE_FROM_WHITE_BAR, move_to, dice);
    }
  }

  if (!game->white_turn && game->board->bar_red > 0) {
    int move_to = dice - 1;

    if (game->board->fields[move_to] <= 1) {
      MoveList_add(moves, MOVE_FROM_RED_BAR, move_to, dice);
    }
  }
}

void MoveList_remove_duplicates(MoveList* self) {
  for (int i = 0; i < self->size; i++) {
    for (int j = i + 1; j < self->size; j++) {
      if (self->moves[i].from == self->moves[j].from &&
          self->moves[i].to == self->moves[j].to) {
        self->moves[j] = self->moves[self->size - 1];
        self->size--;
        j--;
      }
    }
  }
}

bool MoveList_find_taking_move(MoveList* self, Game* game) {
  if (self->size == 0) {
    return false;
  }

  int min = 24;
  int index = -1;

  for (int i = 0; i < self->size; i++) {
    int distance = self->moves[i].to;

    if (game->white_turn) {
      distance = 23 - distance;
    }

    if (distance < min) {
      min = distance;
      index = i;
    }
  }

  self->moves[0] = self->moves[index];
  self->size = 1;
  return true;
}

bool is_taking_move(Move move, Game* game) {
  if (move.to == MOVE_TO_WHITE_COURT || move.to == MOVE_TO_RED_COURT) {
    return false;
  }

  int8_t move_from_value = game->board->fields[move.from],
         move_to_value = game->board->fields[move.to];

  return (move.from < 24 && (move_from_value * move_to_value) < 0) ||
         (move.from == MOVE_FROM_WHITE_BAR && move_to_value < 0) ||
         (move.from == MOVE_FROM_RED_BAR && move_to_value > 0);
}

void MoveList_force_take(MoveList* self, Game* game) {
  MoveList* taking_moves = MoveList_new();

  for (int i = 0; i < self->size; i++) {
    if (is_taking_move(self->moves[i], game)) {
      MoveList_add(taking_moves, self->moves[i].from, self->moves[i].to,
                   self->moves[i].dice);
    }
  }

  if (MoveList_find_taking_move(taking_moves, game)) {
    self->moves[0] = taking_moves->moves[0];
    self->size = 1;
  }
  MoveList_free(taking_moves);
  return;
}

bool can_remove(Game* game) {
  if (game->white_turn) {
    if (game->board->bar_white > 0) {
      return false;
    }
    for (int i = 6; i < 24; i++) {
      if (game->board->fields[i] > 0) {
        return false;
      }
    }
  } else {
    if (game->board->bar_red > 0) {
      return false;
    }
    for (int i = 0; i < 18; i++) {
      if (game->board->fields[i] < 0) {
        return false;
      }
    }
  }

  return true;
}

void force_remove_add_moves(MoveList* self, Game* game, int dice) {
  if (dice == DICE_NONE) {
    return;
  }

  if (game->white_turn) {
    for (int i = 0; i < 6; i++) {
      if (game->board->fields[i] > 0 && i - dice < 0) {
        MoveList_add(self, i, MOVE_TO_WHITE_COURT, dice);
      }
    }
  } else {
    for (int i = 18; i < 24; i++) {
      if (game->board->fields[i] < 0 && i + dice > 23) {
        MoveList_add(self, i, MOVE_TO_RED_COURT, dice);
      }
    }
  }
}

void MoveList_force_remove(MoveList* self, Game* game) {
  if (!can_remove(game)) {
    return;
  }

  MoveList* removing_moves = MoveList_new();

  for (int i = 0; i < MAX_DICE; i++) {
    force_remove_add_moves(removing_moves, game, game->dice[i]);
  }

  if (removing_moves->size > 0) {
    for (int i = 0; i < removing_moves->size; i++) {
      self->moves[i] = removing_moves->moves[i];
    }
    self->size = removing_moves->size;
  }

  MoveList_free(removing_moves);
  return;
}

void moves_from_fields(MoveList* moves, Game* game) {
  if (game->white_turn) {
    for (int i = 23; i >= 0; i--) {
      check_field(game, moves, i);
    }
  } else {
    for (int i = 0; i < 24; i++) {
      check_field(game, moves, i);
    }
  }
}

MoveList* MoveList_from_Game(Game* game) {
  MoveList* moves = MoveList_new();

  for (int i = 0; i < MAX_DICE; i++) {
    check_moves_from_bar(game, moves, game->dice[i]);
  }

  moves_from_fields(moves, game);

  MoveList_force_remove(moves, game);
  MoveList_remove_duplicates(moves);
  MoveList_force_take(moves, game);
  return moves;
}
