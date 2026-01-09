#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include "types.h"

// --- Συναρτήσεις Διαχείρισης Ταμπλό ---
void init_board(Board *b, int size);
void clear_board(Board *b);
void print_board(Board *b);

// --- Συναρτήσεις Λογικής Παιχνιδιού ---
// Επιστρέφει 1 για επιτυχή κίνηση, 0 για παράνομη (π.χ. αυτοκτονία ή κατειλημμένη θέση)
int play_move(Board *b, Color color, char *coords);

// Παράγει μια κίνηση αυτόματα (για την εντολή genmove)
void generate_move(Board *b, Color color);

// Ελέγχει αν μια ομάδα πετρών έχει ελευθερίες
bool has_liberties(Board *b, int r, int c, Color color);

// Αφαιρεί μια αιχμαλωτισμένη ομάδα από το ταμπλό
void remove_group(Board *b, int r, int c, Color color);

// --- Συναρτήσεις Πρωτοκόλλου GTP ---
void handle_help();                                                // Η ειδική εντολή της εκφώνησης
Color string_to_color(char *s);                                    // Μετατρέπει το "black"/"white" σε Color enum
void coord_to_indices(char *coords, int *row, int *col, int size); // Μετατρέπει το "A19" σε indices πίνακα

#endif