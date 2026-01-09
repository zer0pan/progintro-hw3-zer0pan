#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"

int main()
{
    Board game_board;
    // Αρχικοποίηση στο προεπιλεγμένο μέγεθος 19x19
    init_board(&game_board, 19);

    char line[256];
    char cmd[64], arg1[64], arg2[64];

    // Απενεργοποίηση του buffering για να υπάρχει άμεση επικοινωνία
    // με τον διαιτητή ή το GUI (π.χ. gogui)
    setbuf(stdout, NULL);

    while (fgets(line, sizeof(line), stdin))
    {
        // Ανάλυση της γραμμής εισόδου σε εντολή και ορίσματα
        int num_args = sscanf(line, "%s %s %s", cmd, arg1, arg2);
        if (num_args <= 0)
            continue;

        if (strcmp(cmd, "name") == 0)
        {
            printf("= goteam\n\n");
        }
        else if (strcmp(cmd, "version") == 0)
        {
            printf("= 1.0\n\n");
        }
        else if (strcmp(cmd, "protocol_version") == 0)
        {
            printf("= 2\n\n");
        }
        else if (strcmp(cmd, "boardsize") == 0)
        {
            int new_size = atoi(arg1);
            if (new_size > 0 && new_size <= MAX_BOARD)
            {
                init_board(&game_board, new_size);
                printf("= \n\n");
            }
            else
            {
                printf("? unacceptable size\n\n");
            }
        }
        else if (strcmp(cmd, "clear_board") == 0)
        {
            clear_board(&game_board);
            printf("= \n\n");
        }
        else if (strcmp(cmd, "showboard") == 0)
        {
            print_board(&game_board);
        }
        else if (strcmp(cmd, "play") == 0)
        {
            // Η play δέχεται χρώμα και συντεταγμένες (π.χ. play black D4)
            Color c = string_to_color(arg1);
            if (c != EMPTY && play_move(&game_board, c, arg2))
            {
                printf("= \n\n");
            }
            else
            {
                printf("? illegal move\n\n");
            }
        }
        else if (strcmp(cmd, "genmove") == 0)
        {
            // Η genmove ζητά από τη μηχανή να παίξει μια κίνηση
            Color c = string_to_color(arg1);
            if (c != EMPTY)
            {
                generate_move(&game_board, c);
            }
            else
            {
                printf("? unknown color\n\n");
            }
        }
        else if (strcmp(cmd, "help") == 0)
        {
            handle_help();
        }
        else if (strcmp(cmd, "list_commands") == 0)
        {
            printf("= protocol_version\nname\nversion\nboardsize\nclear_board\nshowboard\nplay\ngenmove\nhelp\nquit\n\n");
        }
        else if (strcmp(cmd, "quit") == 0)
        {
            printf("= \n\n");
            break;
        }
        else
        {
            // Για οποιαδήποτε άλλη εντολή που δεν υποστηρίζουμε
            printf("? unknown_command\n\n");
        }
    }

    return 0;
}