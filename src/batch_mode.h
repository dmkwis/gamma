/** @file
 * Interfejs klasy odpowiedzialnej za tryb wsadowy
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef BATCH_MODE_H
#define BATCH_MODE_H
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "gamma.h"

/** @brief Uruchamia tryb wsadowy.
 * Uruchamia tryb wsadowy.
 * @param[in,out] parsed_command - tablica uzywana do zczytywania linii,
 * @param[in] line_count - wartosc mowiaca ile linii tekstu zostalo zczytane,
 * @param[in,out] board   – wskaznik na gre, dla ktorej uruchamiany jest tryb.
 */
void batch_mode_start(uint32_t *parsed_command, uint32_t line_count, gamma_t *board);
#endif //BATCH_MODE_H
