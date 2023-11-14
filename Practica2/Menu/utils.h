/*
 * Created by CarlosCRG19 14/11/23
 */

#ifndef NCOURSES_UTILS_H
#define NCOURSES_UTILS_H
#include <stdbool.h>
#include <string.h>
#include "windows.h"

bool is_empty(char *str);

void write_choice(char * choice, char ***choices, int choice_index, int max_length);

#endif /*NCOURSES_UTILS_H*/
