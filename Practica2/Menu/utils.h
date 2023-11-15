/*
 * Created by CarlosCRG19 14/11/23
 */

#ifndef NCOURSES_UTILS_H
#define NCOURSES_UTILS_H
#include <stdbool.h>
#include <string.h>
#include "windows.h"
#include "odbc.h"

/* multiple functions that help to process simple tasks throgout the code */

/* writes an option to the out_win */
void write_choice(char * choice, char ***choices, int choice_index, int max_length);

/* writes an error to the message window */
void write_error(WINDOW *msg_window, char *err_msg);

/* writes a message of success to the message window */
void write_success(WINDOW *msg_window, char *success_msg);

/* removes the trailing spaces from a string */
void trim_trailing(char * str);


#endif /*NCOURSES_UTILS_H*/
