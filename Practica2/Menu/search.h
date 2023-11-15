/*
 Created by roberto on 3/5/21.
*/

#ifndef NCOURSES_SEARCH_H
#define NCOURSES_SEARCH_H
#include "windows.h"
#include <string.h>
/*#include <unistd.h>*/
void results_search(char * from, char * to, char * date, int * n_choices,
                    char *** choices, int max_length, int max_rows, WINDOW *msg_win, char ** search_flight_ids_1, char ** search_flight_ids_2);

void flight_details(char *flight_id_1, char *flight_id_2, WINDOW *msg_win);

#endif /*NCOURSES_SEARCH_H*/
