/*
 * Created by CarlosCRG19 14/11/23
 */

#ifndef NCOURSES_UTILS_H
#define NCOURSES_UTILS_H
#include <stdbool.h>
#include <string.h>
#include "windows.h"
#include "odbc.h"

void write_choice(char * choice, char ***choices, int choice_index, int max_length);

void write_error(WINDOW *msg_window, char *err_msg);

void write_success(WINDOW *msg_window, char *success_msg);

void trim_trailing(char * str);


#endif /*NCOURSES_UTILS_H*/
