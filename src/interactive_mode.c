/** @file
 * Implementacja interfejsu interactive_mode.h
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */
#include "interactive_mode.h"

/** @brief Wypisuje plansze
 * @param[in] board - wskaznik na gre, ktorej plaszna jest wypisywana,
 * @param[in] posX - pierwszy koordynat pola ktore ma byc podswietlone,
 * @param[in] posY - drugi koordynat pola ktore ma byc podswietlone.
 */
static void print_board_highlighted(gamma_t *board, uint32_t posX, uint32_t posY) {
    char *p = gamma_board(board);

    uint64_t size_of_field = size_of_max_player(board);
    uint64_t height = get_height(board);
    uint64_t width = get_width(board);
    posY = height - 1 - posY;
    uint64_t iter = 0;
    uint64_t size_of_line = width * size_of_field + 1;
    uint64_t last_sign_index = height*size_of_line;

    for(;iter < size_of_line * (posY); ++iter) {
        printf("%c",p[iter]);
    }

    for(uint64_t ptr = 0; ptr < (posX*size_of_field); ++ptr) {
        printf("%c",p[iter++]);
    }

    START_HIGHLIGHT;
    for(uint64_t ptr = 0; ptr < (size_of_field); ++ptr) {
        printf("%c",p[iter++]);
    }
    END_HIGHLIGHT;

    for(;iter <= last_sign_index; ++iter) {
        printf("%c",p[iter]);
    }

    free(p);
}

/** @brief Wypisuje plansze
 * @param[in] board - wskaznik na gre, ktorej plaszna jest wypisywana.
 */
static void print_board(gamma_t *board) {
    char *p = gamma_board(board);
    printf("%s",p);
    free(p);
}

/** @brief Odpowiada za wczytywanie przyciskow.
 * Odpowiada za wczytywanie przyciskow i interpretacje
 * ruchu gracza.
 * @return komunikat o wczytanym przycisku.
 */
static int process_move() {
    char k = getchar();
    if (k == '\033') {
        k = getchar();
        if(k == '[') {
            k = getchar();
            if (k == 'A') {
                return MOVE_UP;
            } else if (k == 'B') {
                return MOVE_DOWN;
            } else if (k == 'C') {
                return MOVE_RIGHT;
            } else if (k == 'D') {
                return MOVE_LEFT;
            }
        }
    } else if (k == '\40') {
        return MOVE;
    } else if (k == 'g' || k == 'G') {
        return GOLDEN_MOVE;
    } else if (k == 'c' || k == 'C') {
        return CONTINUE;
    } else if (k == '\4') {
        return EXIT_GAME;
    }
    return WRONG_CHAR;
}

/** @brief czysci terminal.
 */
static void clear() {
    printf("\033[2J");
    printf("\033[J");
    printf("\033[H");
    printf("\033[1;1H");
}

void interactive_mode_start(gamma_t *board, uint32_t number_of_players) {

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    bool can_anybody_make_a_move = true;
    bool game_running = true;
    uint32_t posX = (get_width(board) - 1)/2;
    uint32_t posY = (get_height(board) - 1)/2;

    while(can_anybody_make_a_move && game_running)
    {
        can_anybody_make_a_move = false;

        for(uint32_t player = 1; player <= number_of_players && game_running; ++player) {

            if(!(gamma_free_fields(board, player) == 0
               && gamma_golden_possible(board, player) == false)) {

                can_anybody_make_a_move = true;

                while(true) {

                    clear();
                    print_board_highlighted(board, posX, posY);
                    printf("PLAYER %u %lu %lu ",
                            player,
                            gamma_busy_fields(board, player),
                            gamma_free_fields(board, player));

                    printf(gamma_golden_possible(board, player) ? "G\n" : "\n");

                    int instruction = process_move();

                    if (instruction == CONTINUE) {
                        break;
                    }
                    else if (instruction == EXIT_GAME) {
                        game_running = false;
                        break;
                    }
                    else if (instruction == MOVE_UP) {
                        if(posY < get_height(board) - 1) posY++;
                    }
                    else if (instruction == MOVE_LEFT) {
                        if(posX > 0) posX--;
                    }
                    else if (instruction == MOVE_DOWN) {
                        if(posY > 0) posY--;
                    }
                    else if (instruction == MOVE_RIGHT) {
                        if(posX < get_width(board) - 1) posX++;
                    }
                    else if (instruction == MOVE) {
                        if(gamma_move(board, player, posX, posY)) {
                            break;
                        }
                    }
                    else if (instruction == GOLDEN_MOVE) {
                        if(gamma_golden_move(board, player, posX, posY)) {
                            break;
                        }
                    }
                }
            }
        }
    }

    clear();

    print_board(board);
    for(uint32_t i = 1; i <= number_of_players; ++i ) {
        printf("PLAYER %u %lu\n", i, fields_taken_by_player(board, i));
    }

    gamma_delete(board);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
