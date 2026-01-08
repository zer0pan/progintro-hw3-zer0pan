#include "helpers.h"

const int dx[4] = {1, -1, 0, 0}; // right left stay stay
const int dy[4] = {0, 0, 1, -1}; // stay stay up down

int on_board(GameState *g, int x, int y)
{
    return (x >= 0 && x < g->size && y >= 0 && y < g->size);
}

int dfs_liberties(GameState *g,
                  int x, int y,
                  int visited[MAX_BOARD][MAX_BOARD],
                  int visited_empty[MAX_BOARD][MAX_BOARD])
{

    int libs = 0;
    visited[x][y] = 1;

    for (int i = 0; i < 4; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (!on_board(g, nx, ny))
            continue;

        if (g->board[nx][ny] == EMPTY)
        {
            if (!visited_empty[nx][ny])
            {
                visited_empty[nx][ny] = 1;
                libs++;
            }
        }
        else if (g->board[nx][ny] == g->board[x][y])
        {
            if (!visited[nx][ny])
            {
                libs += dfs_liberties(g, nx, ny,
                                      visited, visited_empty);
            }
        }
    }

    return libs;
}

void remove_group(GameState *g,
                  int x, int y,
                  int visited[MAX_BOARD][MAX_BOARD])
{
    visited[x][y] = 1;
    g->board[x][y] = EMPTY;

    for (int i = 0; i < 4; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (!on_board(g, nx, ny))
        {
            continue;
        }

        if (visited[nx][ny])
        {
            continue;
        }

        if (g->board[x][y] == g->board[nx][ny])
        {
            remove_group(g, nx, ny, visited);
        }
    }
}

int play_move(GameState *g, int x, int y, Stone color)
{
    if (!on_board(g, x, y))
    {
        return 0;
    }
    if (g->board[x][y] != EMPTY)
    {
        return 0;
    }

    // the rival stone
    Stone opponent = (color == BLACK) ? WHITE : BLACK;

    for (int i = 0; i < 4; i++)
    {
        // new coordinates
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (!on_board(g, nx, ny))
            continue;
        if (g->board[nx][ny] != opponent)
            continue;

        int visited[MAX_BOARD][MAX_BOARD] = {0};
        int visited_empty[MAX_BOARD][MAX_BOARD] = {0};

        int libs = dfs_liberties(g, nx, ny, visited, visited_empty);

        if (libs == 0)
        {
            int visited_remove[MAX_BOARD][MAX_BOARD] = {0};
            remove_group(g, nx, ny, visited_remove);
        }
    }

    // Έλεγχος suicide
    int visited[MAX_BOARD][MAX_BOARD] = {0};
    int visited_empty[MAX_BOARD][MAX_BOARD] = {0};

    int my_libs = dfs_liberties(g, x, y,
                                visited, visited_empty);

    if (my_libs == 0)
    {
        g->board[x][y] = EMPTY;
        return 0;
    }

    g->cons_passes = 0;
    return 1;
}

void init_game(GameState *g, int size)
{
    g->size = size;
    g->cons_passes = 0;
    g->game_over = 0;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            g->board[x][y] = EMPTY;
        }
    }
}

void play_pass(GameState *g)
{
    g->cons_passes++;
    if (g->cons_passes >= 2)
    {
        g->game_over = 1;
    }
}
