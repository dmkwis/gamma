/** @file
 * Zawiera stale do poprawnej interpretacji linijki w trybie wsadowym
 * oraz przed wlaczeniem trybu.
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef BATCH_MODE_AND_PARSER_CONSTANTS_H
#define BATCH_MODE_AND_PARSER_CONSTANTS_H

#define MAX_NUMBER_OF_COMMANDS 5 /**< makro mowiace ile maksymalnie slow
                                   * moze miec poprawna komenda **/
#define COMMENT 0 ///< makro mowiace o tym ze podana linijka byla komentarzem
#define BATCH_MODE 1 ///< makro odpowiadajace za uruchomienie trybu wsadowego
#define INTERACTIVE_MODE 2 /**< makro odpowiadajace za
                            * uruchomienie trybu interaktywnego **/
#define INCORRECT_COMMAND 3 /**< makro mowiace o tym,
                             * ze interpretowana linia byla nieprawidlowa **/
#define EMPTY_LINE 5 /**< makro mowiace o tym, ze zczytano
                      * linijke skladajaca sie z samych bialych znakow **/
#define GAMMA_MOVE 6 ///< makro odpowiedzialne za wykonanie ruchu
#define GAMMA_GOLDEN_MOVE 7 ///< makro odpowiedzialne za wykonanie zlotoego ruchu
#define GAMMA_BUSY_FIELDS 8 /**< makro odpowiedzialne za wywalnie
                             * funkcji gamma_busy_fields **/
#define GAMMA_FREE_FIELDS 9 /**< makro odpowiedzialne za wywalnie
                             * funkcji gamma_free_fields **/
#define GAMMA_GOLDEN_POSSIBLE 10 /**< makro odpowiedzialne za wywalnie
                                    * funkcji gamma_golden_possible **/
#define GAMMA_BOARD 11 /**< makro odpowiedzialne za wywalnie
                        * funkcji gamma_board **/

#endif //BATCH_MODE_AND_PARSER_CONSTANTS_H
