/*
 Created by roberto on 3/5/21.
*/

#ifndef NCOURSES_BPASS_H
#define NCOURSES_BPASS_H

#include "windows.h"
#include <string.h>
/*#include <unistd.h>*/
void results_bpass(SQLHSTMT booking_stmt, SQLHSTMT created_boarding_passes_stmt, char * bookID, int * n_choices,
                   char *** choices, int max_length, int max_rows, WINDOW *msg_win);

#endif /*NCOURSES_BPASS_H */
