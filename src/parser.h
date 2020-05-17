/** @file
 * Interfejs odpowiedzialny z zczytywanie i interpretacje linijek
 * w trybie wsadowym oraz przed wyborem trybu
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <glob.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "batch_mode_and_parser_constants.h"

/** @brief interpretuje znaczenie linii przed wyborem trybu.
 * @param[in] line   – tekst do interpretacji
 * @param[in,out] parsed_command - tablica,
 * w ktorej znajduje sie zinterpretowana wiadomosc.
 */
void process_line(char *line, uint32_t *parsed_command);

/** @brief interpretuje znaczenie linii w trybie wsadowym.
 * @param[in] line   – tekst do interpretacji
 * @param[in,out] parsed_command - tablica,
 * w ktorej znajduje sie zinterpretowana wiadomosc.
 */
void process_line_batch_mode(char *line, uint32_t *parsed_command);

#endif //PARSER_H
