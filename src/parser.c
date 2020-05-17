/** @file
 * Implementacja interfejsu parser.h.
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#include "parser.h"

/** @brief funkcja sprawdza czy slowo sklada sie z samych cyfr.
 * @param[in] word - sprawdzane slowo,
 * @param[in] word_length - dlugosc slowa.
 * @return wartosc @p true jesli slowo sklada sie z samych cyfr, @p false
 * w przeciwnym przypadku.
 */
static bool contains_only_digits(const char *word, size_t word_length) {
    bool valid = true;

    for(unsigned int i = 0; i < word_length; ++i) {
        valid &= (word[i] >='0' && word[i]<='9');
    }

    return valid;
}

/** @brief sprawdza czy liczba reprezentowana przez slowo jest
 * w zakresie typu uint32_t.
 * @param[in] word - liczba reprezentowana przez slowo.
 * @return wartosc @p true jesli liczb reprezentowana przez slowo jest
 * w zakresie typu uint32_t, @p false w przeciwnym przypadku.
 */
static bool in_uint32_t_range(const char *word) {
    return strtoull(word, NULL, 10) <= UINT32_MAX;
}

/** @brief sprawdza czy slowo konczy sie znakiem '\n'.
 * @param[in] line - sprawdzane slowo
 * @return wartosc @p true jesli slowo konczy sie znakiem
 * nowej linii, @p false w przeciwnym przypadku
 */
static bool ends_with_endl(const char *line) {
    return line[strlen(line) - 1] == '\n';
}


/** @brief sprawdza czy slowo sklada sie z samych bialych znakow
 * @param[in] word - sprawdzane slowo,
 * @param[in] word_length - dlugosc sprawdzanego slowa.
 * @return wartosc @p true jesli slowo sklada sie z samych bialych znakow,
 * @p false w przeciwnym przypadku.
 */
static bool is_all_white(const char *word, size_t word_length) {
    bool valid = true;

    for (unsigned int i = 0; i < word_length; ++i) {
        valid &= (word[i] == ' ' || word[i] == '\t' || word[i] == '\r'
                  || word[i] == '\n' || word[i] == '\v' || word[i] == '\f');
    }

    return valid;
}

/** @brief sprawdza czy slowo moze inicjowac jakis tryb.
 * sprawdza czy slowo moze inicjowac jakis tryb.
 * odpowiednio zmienia wartosc w tablicy parsed_command
 * @param[in] word - sprawdzane slowo,
 * @param[in,out] parsed_command - tablica przechowujaca informacje o
 * parsowanej linijce.
 */

static void check_first_word(const char *word, uint32_t *parsed_command) {
    if(strcmp(word, "B") == 0) {
        parsed_command[0] = BATCH_MODE;
    }
    else if(strcmp(word, "I") == 0) {
        parsed_command[0] = INTERACTIVE_MODE;
    }
    else {
        parsed_command[0] = INCORRECT_COMMAND;
    }
}

/** @brief sprawdza czy slowo nie zaczyna sie bialym znakiem.
 * @param[in] word - sprawdzane slowo
 * @return wartosc @p true jesli slowo nie zaczyna sie bialym znakiem,
 * @p false w przeciwnym przypadku.
 */
static bool doesnt_begin_with_white_character(const char* word) {
    char white_chars[] = " \t\r\v\f";

    for(int i = 0; i < 5; ++i) {
        if(white_chars[i] == word[0]) {
            return false;
        }
    }

    return true;
}

void process_line(char *line, uint32_t *parsed_command) {
    char *parsedLine[MAX_NUMBER_OF_COMMANDS];

    if (line[0] == '#' || line[0] == '\n') {
        parsed_command[0] = COMMENT;
    }
    else if (is_all_white(line, strlen(line))) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else if (!ends_with_endl(line)) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else if(!doesnt_begin_with_white_character(line)) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else {

        char delimit[] = " \t\r\n\v\f";
        char *token = strtok(line, delimit);
        unsigned int wordCount = 0;

        while (token) {
            if (wordCount == MAX_NUMBER_OF_COMMANDS) {
                wordCount = MAX_NUMBER_OF_COMMANDS + 1;
                break;
            }

            parsedLine[wordCount++] = token;

            token = strtok(NULL, delimit);
        }

        if (wordCount != MAX_NUMBER_OF_COMMANDS) {
            parsed_command[0] = INCORRECT_COMMAND;
        }
        else {
            check_first_word(parsedLine[0],parsed_command);
            for(unsigned int i = 1; i < wordCount; ++i) {
                if (!contains_only_digits(parsedLine[i], strlen(parsedLine[i]))) {
                    parsed_command[0] = INCORRECT_COMMAND;
                    return;
                }
                else if(!in_uint32_t_range(parsedLine[i])) {
                    parsed_command[0] = INCORRECT_COMMAND;
                    return;
                }
            }

            if(parsed_command[0] == BATCH_MODE || parsed_command[0] == INTERACTIVE_MODE) {
                for(unsigned int i = 1; i < wordCount; ++i) {
                        parsed_command[i] = strtoul(parsedLine[i],NULL,10);
                }
            }
        }

    }
}

/** @brief interpretuje pierwsze slowo w linii podawanej w trybie wsadowym.
 * interpretuje pierwsze slowo w linii podawanej w trybie wsadowym.
 * odpowiednio modyfikuje tablice odpowiedzialna za przekazywanie wiadomosci.
 * @param[in] word - slowo do interpretacji
 * @param[in,out] parsed_command - tablica zawierajaca parsowana wiadomosc.
 */
static void check_first_word_batch_mode(const char *word, uint32_t *parsed_command) {
    if(strcmp(word, "m") == 0) {
        parsed_command[0] = GAMMA_MOVE;
    }
    else if(strcmp(word, "g") == 0) {
        parsed_command[0] = GAMMA_GOLDEN_MOVE;
    }
    else if(strcmp(word, "b") == 0) {
        parsed_command[0] = GAMMA_BUSY_FIELDS;
    }
    else if(strcmp(word, "f") == 0) {
        parsed_command[0] = GAMMA_FREE_FIELDS;
    }
    else if(strcmp(word, "q") == 0) {
        parsed_command[0] = GAMMA_GOLDEN_POSSIBLE;
    }
    else if(strcmp(word, "p") == 0) {
        parsed_command[0] = GAMMA_BOARD;
    }
    else {
        parsed_command[0] = INCORRECT_COMMAND;
    }
}

void process_line_batch_mode(char *line, uint32_t *parsed_command) {
    char *parsedLine[MAX_NUMBER_OF_COMMANDS];

    if (line[0] == '#' || line[0] == '\n') {
        parsed_command[0] = COMMENT;
    }
    else if (is_all_white(line, strlen(line))) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else if (!ends_with_endl(line)) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else if(!doesnt_begin_with_white_character(line)) {
        parsed_command[0] = INCORRECT_COMMAND;
    }
    else {

        char delimit[] = " \t\r\n\v\f";
        char *token = strtok(line, delimit);
        unsigned int wordCount = 0;

        while (token) {
            if (wordCount == MAX_NUMBER_OF_COMMANDS) {
                wordCount = MAX_NUMBER_OF_COMMANDS + 1;
                break;
            }

            parsedLine[wordCount++] = token;

            token = strtok(NULL, delimit);
        }

        if (wordCount > MAX_NUMBER_OF_COMMANDS) {
            parsed_command[0] = INCORRECT_COMMAND;
        }
        else {
            check_first_word_batch_mode(parsedLine[0], parsed_command);

            for (unsigned int i = 1; i < wordCount; ++i) {
                if (!contains_only_digits(parsedLine[i], strlen(parsedLine[i]))) {
                    parsed_command[0] = INCORRECT_COMMAND;
                    return;
                }
                else if(!in_uint32_t_range(parsedLine[i])) {
                    parsed_command[0] = INCORRECT_COMMAND;
                    return;
                }
            }

            if ((parsed_command[0] == GAMMA_MOVE && wordCount != 4) ||
                (parsed_command[0] == GAMMA_GOLDEN_MOVE && wordCount != 4) ||
                (parsed_command[0] == GAMMA_BUSY_FIELDS && wordCount != 2) ||
                (parsed_command[0] == GAMMA_FREE_FIELDS && wordCount != 2) ||
                (parsed_command[0] == GAMMA_GOLDEN_POSSIBLE && wordCount != 2) ||
                (parsed_command[0] == GAMMA_BOARD && wordCount != 1)) {
                parsed_command[0] = INCORRECT_COMMAND;
                return;
            }

            for (unsigned int i = 1; i < wordCount; ++i) {
                parsed_command[i] = strtoul(parsedLine[i], NULL, 10);
            }

        }

    }
}
