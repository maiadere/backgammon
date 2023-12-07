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
    MoveList_add(moves, field, field - dice);
  }

  if (!game->white_turn && game->board->fields[field] < 0 &&
      can_move(game, field, dice)) {
    MoveList_add(moves, field, field + dice);
  }
}

void check_field(Game* game, MoveList* moves, int field) {
  check_field_and_dice(game, moves, field, game->dices[0]);

  if (game->dices[0] != game->dices[1]) {
    check_field_and_dice(game, moves, field, game->dices[1]);
  }

  if (game->dices[0] != DICE_NONE && game->dices[1] != DICE_NONE) {
    check_field_and_dice(game, moves, field, game->dices[0] + game->dices[1]);
  }
}

MoveList* MoveList_from_Game(Game* game) {
  MoveList* moves = MoveList_new();

  if (game->white_turn) {
    for (int i = 23; i >= 0; i--) {
      check_field(game, moves, i);
    }
  } else {
    for (int i = 0; i < 24; i++) {
      check_field(game, moves, i);
    }
  }

  return moves;
}
