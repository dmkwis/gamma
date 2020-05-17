/** @file
 * Implementacja interfejsu gamma.h
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#include <stdio.h>
#include "gamma.h"

/** @struct gamma
 * @brief Struktura przechowująca stan gry.
 * Struktura przechowuje stan gry,
 * informacje na szerokosci i wysokosci planszy,
 * ilosci graczy i mozliwej maksymalnej ilosci rozlacznych obszarow,
 * ilosci obszarow poszczegolnych graczy i ilosc zajmowanych przez nich pol,
 * informacje o tym ktorzy gracze moga wykonac golden move,
 * wskaznik do struktury drzewa find and union
 * oraz zmienne pomocnicze:
 * tablica uzywana do przeszukiwania obszaru w
 * @ref gamma_golden_move oraz @ref check_golden_move
 * i dwie pomocnicze zmienne typu pair_t* uzywane 
 * do przeszukiwania drzewa find and union
 */
struct gamma {
    uint32_t width; ///< szerokosc planszy
    uint32_t height; ///< wysokosc planszy
    uint32_t players; ///< ilosc graczy
    uint32_t areas; ///< maksymalna ilosc rozlacznych obszarow
    uint32_t **board; ///< tablica przechowujaca stan gry
    uint64_t *players_areas; /**<tablica pamietajaca ilosc rozlacznych
                                * obszarow zajmowanych przez
                                * poszczegolnych graczy **/
    uint64_t *players_field_count; /**< tablica pamietajaca ilosc pol
                                  * zajmowanych przez poszczegolnych graczy **/
    bool *possible_golden_move; /**< tablica pamietajaca
                                * o mozliwosci wykonania golden move przez graczy**/
    fau_t *f; ///< struktura przechowujaca find and union
    bool **visited; /**< pomocnicza tablica do przeszukiwania obszaru
                    * w @ref gamma_golden_move oraz @ref check_golden_move **/
    pair_t *a, *b; /**< pomocnicze zmienne typu pair_t*
                    * uzywane do przeszukiwania drzewa find and union **/
};

/**
 * Struktura przechowująca stan gry.
 */
typedef struct gamma gamma_t;

void gamma_delete(gamma_t *g) {
    if(g != NULL) {
        for(uint32_t i = 0; i < g->width; i++) {
            if(g->board != NULL)free(g->board[i]);
            if(g->visited != NULL)free(g->visited[i]);
        }
        free(g->board);
        free(g->visited);
        free(g->players_areas);
        free(g->players_field_count);
        free(g->possible_golden_move);
        delete_fau(g->f);
        delete_pair(g->a);
        delete_pair(g->b);
        free(g);
    }
}

/** @brief funkcja alokujaca pamiec na tablice pamietajaca stan gry.
 * funkcja alokujace pamiec na tablice pamietajaca stan gry, zwraca wiadomosc
 * o powodzeniu tej operacji
 * w razie powodzenia ustawia pola na puste (wartosc 0)
 * @param[in,out] g - wskaznik na strukture gry, dla ktorej alokujemy tablice,
 * @param[in] width - szerokosc gry,
 * @param[in] height - wysokosc gry.
 * @return true jesli udalo zaalokowac pamiec, false w przeciwnym wypadku.
 */
static bool empty_board_init(gamma_t *g, uint32_t width, uint32_t height) {
    uint64_t w = width;
    w*=sizeof(uint32_t *);
    g->board = malloc(w);
    if(g->board == NULL) {
        return false;
    }
    bool flag = true;
    uint64_t h = height;
    h *= sizeof(uint32_t);
    for(uint32_t i = 0; i < width; i++) {
        g->board[i] = malloc(h);
        if(g->board[i] == NULL) flag = false;
    }

    if(flag == false) {
        return false;
    }
    for(uint32_t x = 0; x < width; ++x) {
        for(uint32_t y = 0; y < height; ++y) {
            g->board[x][y] = 0;
        }
    }
    return true;
}

/** @brief funkcja alokujaca pamiec na tablice pamietajaca ilosc obszarow graczy.
 * oraz na tablice pamietajaca ilosc pol zajmowanych przez nich.
 * funkcja alokujaca pamiec na tablice pamietajaca ilosc obszarow graczy
 * oraz na tablice pamietajaca ilosc pol zajmowanych przez nich.
 * Zwraca wiadomosc o powodzeniu tej operacji
 * w razie powodzenia wypelnia obie tablice zerami.
 * @param[in,out] g - wskaznik na strukture gry, dla ktorej alokujemy tablice,
 * @param[in] players - ilosc graczy w grze.
 * @return true jesli udalo zaalokowac pamiec, false w przeciwnym wypadku.
 */
static bool empty_players_areas_player_field_count_init
					(gamma_t *g, uint32_t players) {
    uint64_t p = players;
    g->players_areas = malloc(sizeof(uint64_t) * (p + 1));
    g->players_field_count = malloc(sizeof(uint64_t) * (p + 1));

    if(g->players_areas == NULL || g->players_field_count == NULL) {
        return false;
    }

    for(uint64_t i = 0; i <= p; ++i) {
        g->players_field_count[i] = g->players_areas[i] = 0;
    }
    return true;
}

/** @brief funkcja czyszczaca pomocniczna tablice @p visited.
 * funkcja czyszczaca pomocniczna tablice @p visited.
 * @param[in,out] g - wskaznik na strukture gry,
 * w ktorej trzymana jest tablica visited,
 * @param[in] width - szerokosc gry,
 * @param[in] height - wysokosc gry.
 */
static void clear_visited(gamma_t *g, uint32_t width, uint32_t height) {
    uint64_t w = width;
    uint64_t h = height;
    for(uint64_t x = 0; x < w; ++x) {
        for (uint64_t y = 0; y < h; ++y) {
            g->visited[x][y] = false;
        }
    }
}

/** @brief funkcja alokujaca pamiec na pomocnicza tablice visited.
 * funkcja alokujace pamiec na pomocnicza tablice visited, zwraca wiadomosc
 * o powodzeniu tej operacji
 * w razie powodzenia ustawia pola na false.
 * @param[in,out] g - wskaznik na strukture gry, dla ktorej alokujemy tablice,
 * @param[in] width - szerokosc gry,
 * @param[in] height - wysokosc gry.
 * @return true jesli udalo zaalokowac pamiec, false w przeciwnym wypadku.
 */
static bool empty_visited_init(gamma_t *g, uint32_t width, uint32_t height) {
    uint64_t w = width*sizeof(bool*);
    w*=sizeof(bool*);
    g->visited = malloc(w);
    if(g->visited == NULL) {
        return false;
    }
    bool flag = true;
    for(uint32_t i = 0; i < width; i++) {
        uint64_t h = height;
        h*=sizeof(bool);
        g->visited[i] = malloc(h);
        if(g->visited[i] == NULL) flag = false;
    }
    if(flag == false) {
        return false;
    }
    return true;
}


/** @brief funkcja alokujaca pamiec na tablice pamietajaca 
 * informacje mozliwosci golden move.
 * funkcja alokujaca pamiec na tablice pamietajaca 
 * informacje o wykonanych golden move.
 * Zwraca wiadomosc o powodzeniu tej operacji
 * w razie powodzenia wypelnia tablice wartosciami true.
 * @param[in,out] g - wskaznik na strukture gry, dla ktorej alokujemy tablice,
 * @param[in] players - ilosc graczy w grze.
 * @return true jesli udalo zaalokowac pamiec, false w przeciwnym wypadku
 */
static bool empty_possible_golden_move_init(gamma_t *g, uint32_t players) {
    uint64_t p = players;
    g->possible_golden_move = malloc(sizeof(bool) * (p + 1));

    if(g->possible_golden_move == NULL) {
        return false;
    }

    for(uint64_t i = 0; i <= p; ++i) {
        g->possible_golden_move[i] = true;
    }
    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if(width == 0 || height == 0 || areas == 0 || players == 0) return NULL;
    gamma_t *new_board;
    new_board = malloc(sizeof(gamma_t));
    if(new_board == NULL) {
        return NULL;
    }
    new_board->width = width;
    new_board->height = height;
    new_board->players = players;
    new_board->areas = areas;
    bool flag = true;
    flag &= empty_players_areas_player_field_count_init(new_board, players);
    flag &= empty_possible_golden_move_init(new_board, players);
    flag &= empty_board_init(new_board, width, height);
    flag &= empty_visited_init(new_board, width, height);
    flag &= fau_init(&(new_board->f), width, height);
    flag &= pair_init(&(new_board->a));
    flag &= pair_init(&(new_board->b));
    if(flag == false) {
        gamma_delete(new_board);
        return NULL;
    }
    return new_board;
}

/**@brief funkcja sprawzdajaca czy gra zostala poprawnie zainicjowana.
 * funkcja sprawdzajaca czy gra zostala poprawnie zainicjowana.
 * @param[in] g - wskaznik na gre, ktorej poprawne alkowanie jest sprawdzane.
 * @return @p true jesli gra zostala poprawnie zainicjowana, 
 * @p false w przeciwnym przypadku.
 */
static bool gamma_valid(gamma_t *g) {
    return (g != NULL);
}

/**@brief funkcja sprawdzajaca czy istnieje gracz o danym indeksie w danej grze.
 * funkcja sprawdzajaca czy istnieje gracz o danym indeksie w danej grze.
 * @param[in] g - wskaznik na gre, w ktorej sprawdzane jest istnienie gracza,
 * @param[in] player - index gracza, ktorego istnienie jest sprawdzane.
 * @return true jesli istnieje gracz o danym indexie, false w przeciwnym przypadku.
 */
static bool player_valid(gamma_t *g, uint64_t player) {
    return (player > 0 && player <= g->players);
}

/**@brief sprawdza czy danej grze istnieje pole o danych indexach.
 * sprawdza czy danej grze istnieje pole o danych indexach.
 * @param[in] g - wskaznik na gre, w ktorej sprawdzane jest istnienie pola,
 * @param[in] x - pierwsza wspolrzedna pola,
 * @param[in] y - druga wspolrzedna pola.
 * @return true jesli pole istnieje, false w przeciwnym przypadku.
 */
static bool xy_valid(gamma_t *g, uint32_t x, uint32_t y) {
    return (x < g->width && y < g->height);
}

/**@brief sprawdza czy pole ma sasiada o tym samym numerze gracza.
 * sprawdza czy pole ma sasiada o tym samym numerze gracza.
 * @param[in] g - wskaznik na gre, w ktorej sprawdzane jest istnienie sasiada,
 * @param[in] player - gracz, ktorego sasiedztwo sprawdzamy,
 * @param[in] x - pierwsza wspolrzedna pola,
 * @param[in] y - druga wspolrzedna pola.
 * @return true jesli pole posiada sasiada o tym samym numerze gracza, false wpp.
 */
static bool check_if_around_same_player(gamma_t *g, uint32_t player,
                                        uint32_t x, uint32_t y) {
    uint32_t xs[] = 
	{ x, (x==0 ? x : x - 1), x, ((x==UINT32_MAX || x == g->width - 1) ? x : x + 1) };
    uint32_t ys[] = 
	{ (y==0 ? y : y - 1), y, (y==UINT32_MAX || y == g->height - 1) ? y : y + 1, y };

    for(int i = 0; i < 4; ++i) {
        if(!(xs[i] == x && ys[i] == y) && (g->board[xs[i]][ys[i]] == player)) {
            return true;
        }
    }

    return false;
}

/**@brief laczy pole w drzewie fau z sasiadami o tym samym numerze gracza.
 * laczy pole w drzewie fau z sasiadami o tym samym numerze gracza.
 * @param[in,out] g - wskaznik na gre, w ktorej wykonywanie jest laczenie,
 * @param[in] x - pierwsza wspolrzedna pola,
 * @param[in] y - druga wspolrzedna pola.
 */
static void connect_areas(gamma_t *g, uint32_t x, uint32_t y) {

    uint32_t xs[] = 
	{ x, (x==0 ? x : x - 1), x, ((x==UINT32_MAX || x == g->width - 1) ? x : x + 1) };
    uint32_t ys[] = 
	{ (y==0 ? y : y - 1), y, (y==UINT32_MAX || y == g->height - 1) ? y : y + 1, y };

    for(int i = 0; i < 4; ++i) {
        if(g->board[xs[i]][ys[i]] == g->board[x][y] 
		&& !(xs[i] == x && ys[i] == y) && g->board[x][y] != 0) {
            pair_set_value((g->a), x, y);
            pair_set_value((g->b), xs[i], ys[i]);
            unite(g->f, g->a, g->b);
        }
    }

}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(!(gamma_valid(g) && player_valid(g,player) && xy_valid(g,x,y))) {
        return false;
    } else if(g->board[x][y] != 0) {
        return false;
    }
    else {
        bool new_area = !check_if_around_same_player(g, player, x, y);
        if(g->players_areas[player] == g->areas && new_area == true) {
            return false;
        }
        else {
            g->board[x][y] = player;
            g->players_areas[player] += (uint64_t)new_area;
            g->players_field_count[player]++;
            if(new_area == false) {
                uint32_t xs[] =
                        {x, (x == 0 ? x : x - 1), 
			 x, ((x == UINT32_MAX || x == g->width - 1) ? x : x + 1)};
                uint32_t ys[] =
                            {(y == 0 ? y : y - 1), y, 
			     (y == UINT32_MAX || y == g->height - 1) ? y : y + 1, y};
                uint64_t different_area_counter[] = { 0, 0, 0, 0 };
                for (int i = 0; i < 4; ++i) {
                    if (!(x == xs[i] && y == ys[i]) 
			&& g->board[xs[i]][ys[i]] == player) {
                        bool flag = true;
                        pair_set_value(g->a, xs[i], ys[i]);
                        for (int j = i + 1; j < 4; ++j) {
                            if (!(x == xs[j] && y == ys[j]) 
				&& g->board[xs[j]][ys[j]] == player) {
                                pair_set_value(g->b, xs[j], ys[j]);
                                find(g->f, g->b);
                                if (are_equal(find(g->f, g->a), find(g->f, g->b))) {
                                    flag = false;
                                }
                            }
                        }
                        if (flag) {
                            different_area_counter[i]++;
                        }
                    }
                }
                for (int i = 0; i < 4; ++i) {
                    if(different_area_counter[i] == 1) g->players_areas[player]--;
                }
                g->players_areas[player]++;
            }
            connect_areas(g,x,y);
            return true;
        }
    }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if(!(gamma_valid(g) && player_valid(g,player))) {
        return 0;
    }
    else {
        return g->players_field_count[player];
    }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if(!(gamma_valid(g) && player_valid(g, player))) {
        return false;
    } else if(g->possible_golden_move[player] == false) {
        return false;
    }
    else {
        for(uint32_t i = 1; i <= g->players; ++i) {
            if(i != player && g->players_field_count[i] != 0) {
                return true;
            }
        }
        return false;
    }
}

/**@brief przechodzi obszar nalezacy do jednego gracza
 * przechodzi obszar nalezacy do jednego gracza metoda dfs
 * uzywa pomocniczej tablicy @p visited
 * @param[in] g - gra w ktorej przechodzony jest obszar
 * @param[in] player - gracz, ktorego obszar jest przechodzony
 * @param[in] x - pierwszy koordynat odwiedzanego pola
 * @param[in] y - drugi koordynat odwiedzanego pola
 */
static void DFS(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(g->visited[x][y] == false) {
        g->visited[x][y] = true;
        uint32_t xs[] = 
		{ x, (x == 0 ? x : x - 1), x, 
		((x==UINT32_MAX || x == g->width - 1) ? x : x + 1) };
    	uint32_t ys[] = 
		{(y == 0 ? y : y - 1), y, 
		(y==UINT32_MAX || y == g->height - 1) ? y : y + 1, y };
        for(int i = 0; i < 4; ++i){
            if(g->visited[xs[i]][ys[i]] == false && g->board[xs[i]][ys[i]] == player) {
                DFS(g,player,xs[i],ys[i]);
            }
        }
    }
}

/** @brief Sprawdza czy mozna wykonac złoty ruch.
 * Sprawdza czy mozna wykonac zloty ruch na polu @p x @p y
 * @param[in] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza,
 * @param[in] x       – numer kolumny,
 * @param[in] y       – numer wiersza.
 * @return Wartość @p true, jeśli ruch moze zostac wykonany, a @p false,
 * w przeciwnym przypadku.
 */
static bool check_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(g->board[x][y] == 0 || g->board[x][y] == player 
	|| g->possible_golden_move[player] == false) {
        return false;
    } else
    {
        bool new_area = !check_if_around_same_player(g, player, x, y);
        if(new_area == true && g->players_areas[player] == g->areas) {
            return false;
        } else {
            uint32_t prev_player = g->board[x][y];
            g->board[x][y] = player;
            clear_visited(g, g->width, g->height);
            uint32_t xs[] = { x, (x==0 ? x : x - 1), 
			     x, ((x==UINT32_MAX || x == g->width - 1) ? x : x + 1) };
            uint32_t ys[] = { (y==0 ? y : y-1), y, 
			     (y==UINT32_MAX || y == g->height - 1) ? y : y + 1, y};
            uint64_t different_area_counter[] = { 0, 0, 0, 0 };
            for(int i = 0; i < 4; ++i) {
                if(!(x==xs[i] && y==ys[i])  && g->board[xs[i]][ys[i]]!=0) {
                    bool flag = true;
                    pair_set_value(g->a,xs[i],ys[i]);
                    for(int j = i+1; j < 4; ++j) {
                        if(!(x==xs[j] && y==ys[j])  && g->board[xs[j]][ys[j]]!=0) {
                            pair_set_value(g->b,xs[j], ys[j]);

                            if (are_equal(find(g->f, g->a),  find(g->f, g->b))) {
                                flag = false;
                            }
                        }
                    }
                    if(flag) {
                        different_area_counter[i]++;
                    }
                }
            }
            for(int i = 0; i < 4; ++i) {
                if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                    if(g->visited[xs[i]][ys[i]] == false  && g->board[xs[i]][ys[i]]!=0) {

                        DFS(g,g->board[xs[i]][ys[i]],xs[i],ys[i]);
                        different_area_counter[i]--;
                    }
                }
            }
            g->board[x][y] = prev_player;
            bool is_possible = true;
            for(int i = 0; i<4; ++i) {
                if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                    g->players_areas[g->board[xs[i]][ys[i]]] -= different_area_counter[i];
                }
            }
            for(int i = 0; i<4; ++i) {
                if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                    is_possible &= (g->players_areas[g->board[xs[i]][ys[i]]] <= g->areas);
                }
            }
            for(int i = 0; i<4; ++i) {
                if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                    g->players_areas[g->board[xs[i]][ys[i]]] += different_area_counter[i];
                }
            }
            return is_possible;
        }
    }
}

/**@brief przechodzi obszar nalezacy do jednego gracza i usuwa kazdemu rodzica w drzewie find and union.
 * przechodzi obszar nalezacy do jednego gracza metoda dfs
 * uzywa pomocniczej tablicy @p visited
 * i usuwa kazdemu rodzica w drzewie find and union.
 * @param[in,out] g - gra w ktorej przechodzony jest obszar,
 * @param[in] player - gracz, ktorego obszar jest przechodzony,
 * @param[in] x - pierwszy koordynat odwiedzanego pola,
 * @param[in] y - drugi koordynat odwiedzanego pola.
 */
static void DFS_clear_parent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(g->visited[x][y] == false) {
        set_default_parent(g->f,x,y);
        g->visited[x][y] = true;
        uint32_t xs[] = { x, (x == 0 ? x : x - 1), x, 
			 ((x == UINT32_MAX || x == g->width - 1) ? x : x + 1) };
        uint32_t ys[] = {( y== 0 ? y : y-1), y,
			 (y == UINT32_MAX || y == g->height - 1) ? y : y + 1, y };
        for(int i = 0; i < 4; ++i) {
            if(g->visited[xs[i]][ys[i]] == false && g->board[xs[i]][ys[i]] == player) {
                DFS_clear_parent(g, player, xs[i], ys[i]);
            }
        }
    }
}

/**@brief przechodzi obszar nalezacy do jednego gracza i 
 * usuwa kazdemu rodzica w drzewie find and union.
 * przechodzi obszar nalezacy do jednego gracza metoda dfs
 * uzywa pomocniczej tablicy @p visited
 * i ustawia kazdemu rodzica w drzewie find and union na
 * @p parentx @p parenty
 *
 * @param[in,out] g - gra w ktorej przechodzony jest obszar
 * @param[in] player - gracz, ktorego obszar jest przechodzony
 * @param[in] x - pierwszy koordynat odwiedzanego pola
 * @param[in] y - drugi koordynat odwiedzanego pola
 * @param[in] parentx - pierwszy koordynat pola ojca
 * @param[in] parenty - drugi koordynat pola ojca
 */
static void DFS_set_parent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y,
                                                uint32_t parentx, uint32_t parenty) {
    if(g->visited[x][y] == false) {
        g->visited[x][y] = true;
        uint32_t xs[] =
                { x, (x == 0 ? x : x-1), x, 
		  ((x == UINT32_MAX || x == g->width - 1) ? x : x + 1)};
        uint32_t ys[] =
                { (y == 0 ? y : y - 1), y, 
		  (y == UINT32_MAX || y == g->height - 1) ? y : y + 1, y};
        for(int i = 0; i < 4; ++i) {
            if(g->visited[xs[i]][ys[i]] == false && g->board[xs[i]][ys[i]] == player) {
                pair_set_value(g->a,x,y);
                pair_set_value(g->b,xs[i],ys[i]);
                unite(g->f,g->a,g->b);
                DFS_set_parent(g, player, xs[i], ys[i], parentx, parenty);
            }
        }
    }
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(!(gamma_valid(g) && player_valid(g,player) && xy_valid(g,x,y))) {
        return false;
    }
    else if(check_golden_move(g,player,x,y)) {
        g->possible_golden_move[player] = false;
        uint32_t prev_player = g->board[x][y];
        g->players_field_count[prev_player]--;
        if(check_if_around_same_player(g,prev_player,x,y) == false) {
            g->players_areas[prev_player]--;
        }
        g->players_field_count[player]++;
        g->board[x][y] = player;
        if(check_if_around_same_player(g,player,x,y) == false) {
            g->players_areas[player]++;
        }

        uint32_t xs[] = { x, (x == 0 ? x : x - 1), x, 
                         ((x == UINT32_MAX || x == g->width - 1) ? x : x + 1)};
        uint32_t ys[] = {(y == 0 ? y : y - 1), y, 
                         (y == UINT32_MAX || y == g->height - 1) ? y : y + 1, y};
        uint64_t different_area_counter[] = {0, 0, 0, 0};
        for(int i = 0; i < 4; ++i) {
            if(!(x==xs[i] && y==ys[i])  && g->board[xs[i]][ys[i]]!=0) {
                bool flag = true;
                pair_set_value(g->a,xs[i],ys[i]);
                for(int j = i+1; j < 4; ++j) {
                    if(!(x==xs[j] && y==ys[j])  && g->board[xs[j]][ys[j]]!=0) {
                        pair_set_value(g->b,xs[j], ys[j]);
                        if (are_equal(find(g->f, g->a), find(g->f, g->b))) {
                            flag = false;
                        }
                    }
                }
                if(flag) {
                    different_area_counter[i]++;
                }
            }
        }
        set_default_parent(g->f,x,y);

        clear_visited(g, g->width, g->height);
        for(int i = 0; i < 4; ++i) {
            if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                if(g->visited[xs[i]][ys[i]] == false) {
                    DFS_clear_parent(g,g->board[xs[i]][ys[i]],xs[i],ys[i]);
                }
            }
        }
        clear_visited(g, g->width, g->height);
        for(int i = 0; i < 4; ++i) {
            if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                if(g->visited[xs[i]][ys[i]] == false) {

                    DFS_set_parent(g,g->board[xs[i]][ys[i]],xs[i],ys[i],xs[i],ys[i]);
                    different_area_counter[i]--;
                }
            }
        }
        connect_areas(g,x,y);
        for(int i = 0; i<4; ++i) {
           if(!(x==xs[i] && y==ys[i]) &&
                   (g->board[xs[i]][ys[i]] == player || 
		    g->board[xs[i]][ys[i]] == prev_player)) {

                g->players_areas[g->board[xs[i]][ys[i]]] -= different_area_counter[i];
            }
        }
        return true;
    }
    else {
        return false;
    }
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if(!(gamma_valid(g) && player_valid(g,player))) {
        return 0;
    } else {
        uint64_t allfields;
        if (g->players_areas[player] < g->areas) {
            allfields = (uint64_t) g->width * (uint64_t) g->height;
            for (uint64_t i = 1; i <= g->players; ++i) {
                allfields -= g->players_field_count[i];
            }
        } else {
            allfields = (uint64_t) g->width * (uint64_t) g->height;
            for (uint64_t i = 1; i <= g->players; ++i) {
                allfields -= g->players_field_count[i];
            }
            allfields = 0;
            for (uint32_t x = 0; x < g->width; ++x) {
                for (uint32_t y = 0; y < g->height; ++y) {
                    if (g->board[x][y] == 0 && 
			check_if_around_same_player(g, player, x, y) == true) {
                        allfields++;
                    }
                }
            }
        }
        return allfields;
    }
}


/**@brief funkcja liczaca ile cyfr ma liczba.
 * funkcja liczaca ile cyfr ma liczba.
 * @param[in] number - liczba ktorej ilosc cyfr jest liczona.
 * @return ilosc cyfr liczby.
 */
static uint64_t num_of_digits(uint32_t number){
    uint64_t counter = 0;
    while(number > 0) {
        ++counter;
        number/=10;
    }
    return (counter == 0 ? 1 : counter);
}

/**@brief funkcja zwracajaca wieksza z dwoch liczb.
 * funkcja zwracajaca wieksza z dwoch liczb.
 * @param[in] a - pierwsza liczba,
 * @param[in] b - druga liczba.
 * @return max(a,b).
 */
static uint64_t max(uint64_t a, uint64_t b) {
    return ((a < b) ? b : a);
}

char* gamma_board(gamma_t *g) {
    if(!gamma_valid(g)) {
        return NULL;
    }
    else {
        uint64_t count_signs = 1 + (uint64_t) g->height;
        uint64_t max_len = 1;
        for (int64_t y = (int32_t) (g->height - 1); y >= 0; --y) {
            for (uint32_t x = 0; x < g->width; ++x) {
                max_len = max(max_len, num_of_digits(g->board[x][y]));
            }
        }
        char *buffor;
	uint64_t w, h;
	w = g->width;
	h = g->height;
        buffor = malloc(sizeof(char) * 
			((count_signs) + w * h * (uint64_t) max_len));
        if (buffor == NULL) {
            return NULL;
        } 
	else {
            uint64_t ptr = 0, startptr;
            for (int32_t y = (int64_t) (g->height - 1); y >= 0; --y) {
                for (uint32_t x = 0; x < g->width; ++x) {
                    startptr = ptr;
                    ptr += max_len - 1;
                    if (g->board[x][y] == 0) {
                        buffor[ptr--] = '.';
                    }
                    uint64_t temp = g->board[x][y];
                    while (temp > 0) {
                        buffor[ptr--] = (char) (temp % 10 + '0');
                        temp /= 10;
                    }
                    ptr++;
                    while (ptr > startptr) {
                        buffor[--ptr] = ' ';
                    }
                    ptr += max_len;
                }
                buffor[ptr++] = '\n';
            }
            buffor[ptr] = '\0';

            return buffor;
        }
    }
}

uint32_t get_height(gamma_t *g) {
    return g->height;
}

uint32_t get_width(gamma_t *g) {
    return g->width;
}

uint64_t fields_taken_by_player(gamma_t *g, uint32_t player) {
    if (player == 0 || player > g->players) {
        return 0;
    }
    else {
        return g->players_field_count[player];
    }
}

uint32_t size_of_max_player(gamma_t *g) {
    for(int i = g->players; i >= 1; --i) {
        if(g->players_field_count[i] != 0) return num_of_digits(i);
    }
    return 1;
}
