/** @file
 * Implementacja interfejsu fau.h
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 15.04.2020
 */

#include "fau.h"

/** @struct pair
 * @brief Struktura przechowująca pare koordynatow.
 * struktura przechowujaca pare koordynatow @p x i @p y
 * oba typu uint32_t
 */
struct pair {
    uint32_t x; ///< pierwszy koordynat
    uint32_t y; ///< drugi koordynat
};

/**
 * Struktura przechowujaca pare koordynatow typu uint32_t
 */
typedef struct pair pair_t;

bool pair_init(pair_t **p) {
    *p = malloc(sizeof(pair_t));
    if(*p == NULL) return false;
    else return true;
}

void delete_pair(pair_t *p) {
    free(p);
}

bool are_equal(pair_t *a, pair_t *b) {
    return (a->x == b->x) && (a->y == b->y);
}

void pair_set_value (pair_t *p, uint32_t x, uint32_t y) {
    (p)->x = x;
    (p)->y = y;
}

/** @struct fau
 * @brief Struktura przechowująca find and union
 * struktura przechowuje drzewo find and union
 * pamieta szerokosc drzewa,
 * informacje o ojcu kazdego pola oraz
 * o wielkosci drzewa kazdego pola
 */
struct fau {
    uint32_t width; ///< szerokosc drzewa
    pair_t **parent; ///< informacje o ojcu kazdego pola
    uint32_t **size; ///< informacje o drzewa kazdego pola
};

/**
 * Struktura przechowujaca drzewo find and union w dwuwymiarowej tablicy.
 */
typedef struct fau fau_t;

bool fau_init(fau_t **f, uint32_t width, uint32_t height) {

    (*f) = malloc(sizeof(fau_t));

    if(*f == NULL) return false;

    (*f)->parent = malloc(width * sizeof(pair_t *));
    (*f)->size = malloc(width * sizeof(uint32_t *));
    (*f)->width = width;

    if((*f)->size == NULL || (*f)->parent == NULL) {
        return false;
    }

    for(uint32_t x = 0; x < width; ++x) {
        (*f)->parent[x] = malloc(sizeof(pair_t) * height);
        (*f)->size[x] = malloc(sizeof(uint32_t) * height);
    }

    for(uint32_t x = 0; x < width; ++x) {
        if((*f)->parent[x] == NULL || (*f)->size[x] == NULL) {
            return false;
        }
    }


    for(uint32_t x = 0; x < width; ++x) {
        for(uint32_t y = 0; y < height; ++y) {
            set_default_parent(*f, x, y);
        }
    }

    return true;
}

void delete_fau(fau_t *f) {
    if(f != NULL) {
        for(uint32_t x = 0; x < f->width; ++x) {
            if(f->parent != NULL)free(f->parent[x]);
            if(f->size != NULL)free(f->size[x]);
        }
        free(f->parent);
        free(f->size);
        free(f);
    }
}

void set_default_parent(fau_t *f, uint32_t x, uint32_t y) {
    f->parent[x][y].x = x;
    f->parent[x][y].y = y;
    f->size[x][y] = 0;
}

pair_t *find(fau_t *f, pair_t *p) {
    pair_t temp;
    temp.x = p->x;
    temp.y = p->y;
    if (are_equal(&(f->parent[p->x][p->y]),&temp)) {
        return p;
    }
    else {
        f->parent[p->x][p->y] = *find(f, &f->parent[p->x][p->y]);;
        return &(f->parent[p->x][p->y]);
    }
}

void unite(fau_t *f, pair_t *a, pair_t *b) {
    a = find(f, a);
    b = find(f, b);
    if(are_equal(a, b)) return;
    if(f->size[a->x][a->y] > f->size[b->x][b->y]) {
        f->parent[b->x][b->y] = *a;
    } 
    else {
        f->parent[a->x][a->y] = *b;
        if(f->size[a->x][a->y] == f->size[b->x][b->y]) {
            f->size[b->x][b->y]++;
        }
    }
}
