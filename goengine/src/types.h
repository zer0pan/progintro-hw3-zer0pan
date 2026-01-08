#define MAX_BOARD 19

typedef struct
{
    int size;
    Stone board[MAX_BOARD][MAX_BOARD];

    int cons_passes; // 0,1 or 2
    int game_over;   // 0 or 1
} GameState;

typedef enum
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
} Stone;