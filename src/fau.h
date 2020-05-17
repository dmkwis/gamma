/** @file
 * Interfejs klasy drzewa find and union
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 15.04.2020
 */

#ifndef FAU_H
#define FAU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Struktura przechowujaca pare koordynatow typu uint32_t
 */
typedef struct pair pair_t;

/** @brief Tworzy strukturę przechowującą pare koordynatow 
 * oraz przypisuje ja odpowiedniego wskaznika.
 * Alokuje pamięć na nową strukturę przechowującą 
 * pare koordynatow oraz przypisuje ja do wskaznika @p p.
 * @param[in] p – wskaznik na wskaznik, na ktory bedzie 
 * zaalokowana pamiec  do nowo powstalej pary.
 * @return Wartosc true jesli udalo zaalokowac sie pamiec 
 * na pare koordynatow, false w przeciwnym wypadku.
 */
bool pair_init(pair_t **p);

/** @brief Usuwa strukture przechowujaca pare koordynatow
 * Usuwa z pamięci strukturę wskazywaną przez @p p.
 * Nic nie robi jesli ten wskaznik ma wartosc NULL.
 * @param[in] p – wskaznik na pamiec, ktora bedzie zwolniona
 */
void delete_pair(pair_t *p);

/** @brief Sprawdza czy dwie pary koordynatow, sa takie same.
 * Sprawdza czy dwie struktury przechowujace pary koordynatow, 
 * wskazywane przez wskazniki @p a i @p b sa takie same.
 * @param[in] a – wskaznik na pierwsza strukture przechowujaca pare koordynatow.
 * @param[in] b – wskaznik na druga strukture przechowujaca pare koordynatow.
 * @return Wartosc true, jesli struktury sa takie same, false w przeciwnym wypadku.
 */
bool are_equal(pair_t *a, pair_t *b);

/** @brief Ustawia wartosci pol struktury pary koordynatow.
 * ustawia wartosc pol struktury pary koordynatow 
 * wskazywanej przez wskaznik @p p, na odpowiednio
 * @p x i @p y
 * @param[in] p – wskaznik na strukture przechowujaca pare koordynatow,
 * @param[in] x – zmienna, ktorej wartosc bedzie przypisana 
 * do pierwszego pola struktury wskazywanej przez @p p, 
 * @param[in] y – zmienna, ktorej wartosc bedzie przypisana 
 * do drugiego pola struktury wskazywanej przez @p p. 
 */
void pair_set_value (pair_t *p, uint32_t x, uint32_t y);

/**
 * Struktura przechowujaca drzewo find and union w dwuwymiarowej tablicy.
 */
typedef struct fau fau_t;

/** @brief Tworzy strukturę przechowującą drzewo find and union 
 * oraz przypisuje ja odpowiedniego wskaznika.
 * Alokuje pamięć na nową strukturę przechowującą drzewo find and union 
 * oraz przypisuje ja do wskaznika @p f.
 * @param[in] f – wskaznik na wskaznik, na ktory 
 * bedzie zaalokowana pamiec  do nowo powstalej pary.
 * @param[in] width – zmienna, ktorej wartosc ustala szerokosc 
 * drzewa find and union.
 * @param[in] height – wzmienna, ktorej wartosc ustala wysokosc 
 * drzewa find and union.
 * @return Wartosc true jesli udalo zaalokowac sie pamiec 
 * na drzewo find and union, false w przeciwnym wypadku.
 */
bool fau_init(fau_t **f, uint32_t width, uint32_t height);

/** @brief Usuwa strukture przechowujaca drzewo find and union.
 * Usuwa z pamięci strukturę wskazywaną przez @p f.
 * Nic nie robi jesli ten wskaznik ma wartosc NULL.
 * @param[in] f  – wskaznik na pamiec, ktora bedzie zwolniona.
 */
void delete_fau(fau_t *f);

/** @brief Ustawia domyslna wartosci pol struktury drzewa 
 * find and union definiujace odpowiednie pola.
 * Ustawia domyslna wartosc pol struktury drzewa 
 * find and union wskazywanego przez wskaznik @p f
 * definiujace pole @p x @p y.
 * @param[in] f – wskaznik na strukture przechowujaca drzewo find and union.
 * @param[in] x – zmienna, ktorej wartosc jest pierwszym koordynatem 
 * do ustawianego pola.
 * @param[in] y – zmienna, ktorej wartosc jest drugim koordynatem 
 * do ustawianego pola.
 */
void set_default_parent(fau_t *f, uint32_t x, uint32_t y);

/** @brief Znajduje wskaznik na strukture bedaca szukanym ojcem 
 * w drzewie find and union.
 * Znajduje wskaznik na strukture pary koordynatow bedaca ojcem w drzuwie
 * @param[in] f – wskaznik na drzewo find and union, 
 * w ktorym poszukujemy ojca pary wskazywanej prze @p p
 * @param[in] p - wskaznik na strukture, 
 * ktorej ojca poszukujemy w drzewie wskazywanym przez @p f
 * @return Wskaznik na strukture bedaca szukanym ojcem w drzewie find and union
 */
pair_t *find(fau_t *f, pair_t *p);

/** @brief Laczy dwa zbiory w drzewie find and union.
 * Laczy dwa zbiory (w ktorych znajduja sie wartosci 
 * wskaznikow @p a oraz @p b), w drzewie find and union wskazywanym
 * przez @p f.
 * @param[in] f – wskaznik na drzewo find and union, 
 * w zlaczane beda dwa zbiory.
 * @param[in] a - wskaznik na strukture, ktorej zbior 
 * bedzie laczany w drzewie find and union wskazywanym przez @p f.
 * @param[in] b - wskaznik na strukture, ktorej zbior 
 * bedzie laczany w drzewie find and union wskazywanym przez @p f.
 */
void unite(fau_t *f, pair_t *a, pair_t *b);


#endif
