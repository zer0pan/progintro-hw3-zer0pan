#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include "helpers.h"

// Βοηθητική δομή για την ανίχνευση ελευθεριών
bool visited[MAX_BOARD][MAX_BOARD];

void init_board(Board *b, int size)
{
    b->size = (size > MAX_BOARD) ? MAX_BOARD : size;
    b->komi = 7.5; // Τυπικό κομί για Κινέζικους κανόνες
    clear_board(b);
}

void clear_board(Board *b)
{
    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
        {
            b->grid[i][j] = EMPTY;
        }
    }
}

// Ελέγχει αν μια ομάδα πετρών έχει έστω και μία ελευθερία
bool has_liberties(Board *b, int r, int c, Color color)
{
    if (r < 0 || r >= b->size || c < 0 || c >= b->size)
        return false;
    if (b->grid[r][c] == EMPTY)
        return true;
    if (b->grid[r][c] != color || visited[r][c])
        return false;

    visited[r][c] = true;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++)
    {
        if (has_liberties(b, r + dr[i], c + dc[i], color))
            return true;
    }
    return false;
}

// Αφαιρεί μια ομάδα πετρών που αιχμαλωτίστηκε
void remove_group(Board *b, int r, int c, Color color)
{
    if (r < 0 || r >= b->size || c < 0 || c >= b->size || b->grid[r][c] != color)
        return;
    b->grid[r][c] = EMPTY;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++)
        remove_group(b, r + dr[i], c + dc[i], color);
}

int play_move(Board *b, Color color, char *coords)
{
    if (strcasecmp(coords, "pass") == 0)
        return 1;

    int r, c;
    coord_to_indices(coords, &r, &c, b->size);
    if (r < 0 || r >= b->size || c < 0 || c >= b->size || b->grid[r][c] != EMPTY)
        return 0;

    // Προσωρινή τοποθέτηση
    b->grid[r][c] = color;
    Color opponent = (color == BLACK) ? WHITE : BLACK;
    bool captured = false;

    // Έλεγχος αν η κίνηση αιχμαλωτίζει πέτρες του αντιπάλου
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++)
    {
        memset(visited, 0, sizeof(visited));
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < b->size && nc >= 0 && nc < b->size && b->grid[nr][nc] == opponent)
        {
            if (!has_liberties(b, nr, nc, opponent))
            {
                remove_group(b, nr, nc, opponent);
                captured = true;
            }
        }
    }

    // Έλεγχος αυτοκτονίας (Suicide rule)
    memset(visited, 0, sizeof(visited));
    if (!captured && !has_liberties(b, r, c, color))
    {
        b->grid[r][c] = EMPTY; // Αναίρεση κίνησης
        return 0;              // Παράνομη κίνηση
    }

    return 1;
}

void print_board(Board *b)
{
    printf("= \n   ");
    for (int i = 0; i < b->size; i++)
    {
        char col = (i >= 8) ? 'A' + i + 1 : 'A' + i;
        printf("%c ", col);
    }
    printf("\n");
    for (int i = b->size - 1; i >= 0; i--)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < b->size; j++)
        {
            if (b->grid[i][j] == BLACK)
                printf("X ");
            else if (b->grid[i][j] == WHITE)
                printf("O ");
            else
                printf(". ");
        }
        printf("%d\n", i + 1);
    }
    printf("   ");
    for (int i = 0; i < b->size; i++)
    {
        char col = (i >= 8) ? 'A' + i + 1 : 'A' + i;
        printf("%c ", col);
    }
    printf("\n\n");
}

// Υλοποίηση της ειδικής εντολής help βάσει εκφώνησης
void handle_help()
{
    // Αρχικοποίηση γεννήτριας τυχαίων αριθμών
    srand(time(NULL));

    // Έλεγχος του χαμηλότερου bit (lowest bit)
    if ((rand() & 1) == 0)
    {
        printf("= Don't Panic, Just Go\n\n");
    }
    else
    {
        printf("= Go Big or Go Home\n\n");
    }
}

// Μετατροπή κειμένου (GTP argument) σε τύπο Color
Color string_to_color(char *s)
{
    if (s == NULL)
        return EMPTY;

    // Μετατροπή σε πεζά για ευελιξία
    if (strcasecmp(s, "black") == 0 || strcasecmp(s, "b") == 0)
    {
        return BLACK;
    }
    else if (strcasecmp(s, "white") == 0 || strcasecmp(s, "w") == 0)
    {
        return WHITE;
    }
    return EMPTY;
}

// Μετατροπή συντεταγμένων GTP (π.χ. "A19") σε δείκτες πίνακα C (row, col)
void coord_to_indices(char *coords, int *row, int *col, int size)
{
    if (coords == NULL || strlen(coords) < 2)
    {
        *row = -1;
        *col = -1;
        return;
    }

    // Το Go παραλείπει το γράμμα 'I' στις στήλες για να μην μπερδεύεται με το '1'
    char first_char = toupper(coords[0]);
    if (first_char < 'I')
    {
        *col = first_char - 'A';
    }
    else
    {
        *col = first_char - 'A' - 1;
    }

    // Οι γραμμές στο Go ξεκινούν από το 1 στο κάτω μέρος (π.χ. A1 είναι grid[0][0])
    *row = atoi(&coords[1]) - 1;
}

int count_liberties(Board *b, int r, int c)
{
    int libs = 0;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++)
    {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < b->size && nc >= 0 && nc < b->size)
        {
            if (b->grid[nr][nc] == EMPTY)
                libs++;
        }
    }
    return libs;
}

typedef struct
{
    int r, c;
} Move;

void generate_move(Board *b, Color color)
{
    Move legal_moves[MAX_BOARD * MAX_BOARD];
    int count = 0;
    char coords[10];

    // 1. Καταγραφή όλων των νόμιμων κινήσεων
    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
        {
            if (b->grid[i][j] == EMPTY)
            {
                // Μετατροπή σε string για να την ελέγξουμε με την play_move
                char col_char = (j >= 8) ? 'A' + j + 1 : 'A' + j;
                sprintf(coords, "%c%d", col_char, i + 1);

                // Κάνουμε μια δοκιμή σε ένα προσωρινό αντίγραφο του ταμπλό
                Board temp = *b;
                if (play_move(&temp, color, coords))
                {
                    legal_moves[count].r = i;
                    legal_moves[count].c = j;
                    count++;
                }
            }
        }
    }

    // 2. Επιλογή μιας κίνησης
    if (count > 0)
    {
        srand(time(NULL));
        int index = rand() % count; // Τυχαία επιλογή από τις νόμιμες
        int r = legal_moves[index].r;
        int c = legal_moves[index].c;

        char col_char = (c >= 8) ? 'A' + c + 1 : 'A' + c;
        sprintf(coords, "%c%d", col_char, r + 1);

        // Εκτελούμε την κίνηση στο πραγματικό ταμπλό
        play_move(b, color, coords);

        // Εκτύπωση για το GTP
        printf("= %s\n\n", coords);
    }
    else
    {
        // Αν δεν υπάρχει καμία νόμιμη κίνηση
        printf("= pass\n\n");
    }
}