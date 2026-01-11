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
    b->komi = 6.5; // Τυπικό κομί για Κινέζικους κανόνες
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
int remove_group(Board *b, int r, int c, Color color)
{
    if (r < 0 || r >= b->size || c < 0 || c >= b->size || b->grid[r][c] != color)
        return 0;
    b->grid[r][c] = EMPTY;
    return 1 + remove_group(b, r - 1, c, color) + remove_group(b, r + 1, c, color) +
           remove_group(b, r, c - 1, color) + remove_group(b, r, c + 1, color);
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
                int caps = remove_group(b, nr, nc, opponent);
                if (color == BLACK)
                    b->black_caps += caps;
                else
                    b->white_caps += caps;
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

// Βοηθητική συνάρτηση για μέτρηση συνολικών πετρών ενός χρώματος στο ταμπλό
int count_stones(Board *b, Color c)
{
    int count = 0;
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size; j++)
            if (b->grid[i][j] == c)
                count++;
    return count;
}

// Ελέγχει αν μια θέση είναι "μάτι" για ένα χρώμα
int is_eye(Board *b, int r, int c, Color color)
{
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr < 0 || nr >= b->size || nc < 0 || nc >= b->size)
            count++; // Τα όρια μετράνε ως προστασία
        else if (b->grid[nr][nc] == color)
            count++;
    }
    return (count == 4);
}

void generate_move(Board *b, Color color)
{
    // Αρχικοποιούμε το best_score με μια τιμή που αντιπροσωπεύει το PASS.
    // Αν καμία κίνηση δεν είναι καλύτερη από το "τίποτα", τότε το score παραμένει χαμηλά.
    int best_score = -500;
    int best_r = -1, best_c = -1;
    Color opponent = (color == BLACK) ? WHITE : BLACK;

    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
        {
            if (b->grid[i][j] == EMPTY)
            {
                char coords[10];
                char col_char = (j >= 8) ? 'A' + j + 1 : 'A' + j;
                sprintf(coords, "%c%d", col_char, i + 1);

                Board temp_board = *b;
                if (play_move(&temp_board, color, coords))
                {
                    int current_score = 0;

                    // 1. Έλεγχος Αιχμαλωσίας (Capture)
                    int opp_before = count_stones(b, opponent);
                    int opp_after = count_stones(&temp_board, opponent);
                    int captured = opp_before - opp_after;
                    current_score += captured * 1000;

                    // 2. Έλεγχος Αυτοσυντήρησης (Self-Preservation)
                    // Αν η κίνηση που παίξαμε οδηγεί σε δική μας αιχμαλωσία στην επόμενη κίνηση
                    // (Πολύ απλό heuristic: αν η νέα πέτρα έχει μόνο 1 ελευθερία)
                    if (count_liberties(&temp_board, i, j) == 1 && captured == 0)
                    {
                        current_score -= 800; // Μεγάλη ποινή για κίνηση "αυτοκτονίας"
                    }

                    // 3. Στρατηγική Περιοχής (Territory)
                    int center = b->size / 2;
                    int dist = abs(i - center) + abs(j - center);
                    current_score += (b->size - dist); // Bonus για κινήσεις κοντά στο κέντρο

                    // 4. Αποφυγή πλήρωσης δικών μας "ματιών" (Eyes)
                    // Αν μια κίνηση δεν αιχμαλωτίζει και μειώνει τις δικές μας ελευθερίες σε κλειστή περιοχή
                    if (is_eye(b, i, j, color) && captured == 0)
                    {
                        current_score -= 600;
                    }

                    if (current_score > best_score)
                    {
                        best_score = current_score;
                        best_r = i;
                        best_c = j;
                    }
                }
            }
        }
    }

    // Αν το best_score παραμένει κάτω από το όριο ή δεν βρέθηκε καμία νόμιμη κίνηση
    if (best_r != -1 && best_score > -200)
    {
        char final_coords[10];
        char col_char = (best_c >= 8) ? 'A' + best_c + 1 : 'A' + best_c;
        sprintf(final_coords, "%c%d", col_char, best_r + 1);

        play_move(b, color, final_coords);
        printf("= %s\n\n", final_coords);
    }
    else
    {
        // Η μηχανή κρίνει ότι καμία κίνηση δεν είναι ωφέλιμη
        printf("= pass\n\n");
    }
}

float calculate_score(Board *b, Color color)
{
    float score = (color == WHITE) ? b->komi : 0; // Ο Λευκός ξεκινά με το Komi (7.5)

    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
        {
            if (b->grid[i][j] == color)
            {
                score += 1.0; // Μονάδα για κάθε πέτρα
            }
            else if (b->grid[i][j] == EMPTY)
            {
                // Εδώ θα μπορούσε να μπει έλεγχος για περιοχή (territory)
                // Για την εργασία, οι περισσότεροι μετρούν πέτρες + αιχμάλωτους
                // ή απλά τις πέτρες αν το παιχνίδι τελειώσει σωστά.
            }
        }
    }
    return score;
}