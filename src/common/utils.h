//
// Created by user on 20.06.2021.
//

#ifndef SPO_LAB_1_5_UTILS_H
#define SPO_LAB_1_5_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#ifdef __WIN32
size_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

#define DEBUG 1
#define debugf(f_, ...) ({ if (DEBUG) { printf((f_), __VA_ARGS__); } })
#define debugp(f_) ({ if (DEBUG) { printf(f_); } })
#endif //SPO_LAB_1_5_UTILS_H
