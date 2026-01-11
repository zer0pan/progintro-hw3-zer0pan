#ifndef TYPES_H
#define TYPES_H

#define MAX_BOARD 19

// Ορισμός χρωμάτων για τις πέτρες και τις κενές θέσεις
typedef enum
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
} Color;

// Η δομή που αναπαριστά την κατάσταση του παιχνιδιού
typedef struct
{
    int size;                         // Διάσταση (π.χ. 19 για 19x19)
    Color grid[MAX_BOARD][MAX_BOARD]; // Ο πίνακας με τις πέτρες
    double komi;                      // Επιπλέον βαθμοί για τον Λευκό (Κινέζικοι κανόνες: 7.5)
    int black_caps;                   // Πέτρες που έφαγε ο Μαύρος (άρα χάνει πόντους ο Λευκός)
    int white_caps;                   // Πέτρες που έφαγε ο Λευκός (άρα χάνει πόντους ο Μαύρος)
} Board;

#endif