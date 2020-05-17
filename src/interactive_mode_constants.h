/** @file
 * Zawiera stale potrzebne do poprawnego funkcjonowania interfejsu
 * interactive_mode.h.
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef INTERACTIVE_MODE_CONSTANTS_H
#define INTERACTIVE_MODE_CONSTANTS_H

#define START_HIGHLIGHT printf("\x1b[44m"); /**< makro odpowiadajace za
                                            * poczatek podswietlenia tekstu **/
#define END_HIGHLIGHT printf("\x1b[0m"); /**< makro odpowiadajace za
                                            * koniec podswietlenia tekstu **/
#define MOVE_UP 0 ///< makro odpowiadajace za ruch w gore
#define MOVE_DOWN 1 ///< makro odpowiadajace za ruch w dol
#define MOVE_RIGHT 2 ///< makro odpowiadajace za ruch w prawo
#define MOVE_LEFT 3 ///< makro odpowiadajace za ruch w lewo
#define MOVE 4 ///< makro odpowiadajace za wykonanie ruchu
#define GOLDEN_MOVE 5 ///< makro odpowiadajace za wykonanie zlotego ruchu
#define CONTINUE 6 ///< makro odpowiadajace za pominiecie tury
#define EXIT_GAME 7 ///< makro odpowiadajace za konca gry
#define WRONG_CHAR 8 ///< makro odpowiadajace za wcisniecie zlego znaku

#endif //INTERACTIVE_MODE_CONSTANTS_H
