/** @file
 * Interfejs klasy odpowiedzialnej za tryb interaktywny
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef INTERACTIVE_MODE_H
#define INTERACTIVE_MODE_H
#include <stdint.h>
#include <stdio.h>
#include "gamma.h"
#include <termio.h>
#include <unistd.h>
#include "interactive_mode_constants.h"

/** @brief Uruchamia tryb interaktywny.
 * Uruchamia tryb interaktywny.
 * @param[in,out] board   – wskaznik na gre, dla ktorej uruchamiany jest tryb,
 * @param[in] number_of_players  - ilosc graczy, liczba dodatnia.
 */
void interactive_mode_start(gamma_t *board, uint32_t number_of_players);
#endif //INTERACTIVE_MODE_H
