#pragma once

#include <ncurses.h>

#define cprintw(color, ...)  \
  attron(COLOR_PAIR(color)); \
  printw(__VA_ARGS__);       \
  attron(COLOR_PAIR(7));

inline void nspaces(int n) {
  for (int i = 0; i < n; i++) {
    addch(' ');
  }
}

#define WHITE_PAWN_STR "\u2B24"
#define RED_PAWN_STR "\u25EF"

#define DEFAULT_FIELD_STR1 "\u1362"
#define DEFAULT_FIELD_STR2 "\u1368"

#define DEFAULT_COURT_FIELD_STR "\u00B7"
