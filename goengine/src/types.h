#define MAX_BOARD 19

typedef struct
{
    int size;
    Stone board[MAX_BOARD][MAX_BOARD];
} GameState;

typedef enum
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
} Stone;