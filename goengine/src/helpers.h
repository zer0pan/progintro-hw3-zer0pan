#include "types.h"

int play_move(GameState *g, int x, int y, Stone color);
int on_board(GameState *g, int x, int y);
void init_game(GameState *g, int size);
void play_pass(GameState *g);