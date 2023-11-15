/*
* Created by roberto on 3/5/21.
*/
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlucode.h>
#include <stdio.h>
#include "lbpass.h"
#include "odbc.h"
#include "utils.h"

void    results_bpass(SQLHSTMT booking_stmt, SQLHSTMT created_boarding_passes_stmt, char * bookID,
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows,
                       WINDOW *msg_win)
/**here you need to do your query and fill the choices array of strings
*
* @param bookID  form field bookId
* @param n_choices fill this with the number of results
* @param choices fill this with the actual results
* @param max_length output win maximum width
* @param max_is output win maximum number of rows
*/

{
    SQLCHAR passenger_name[32], flight_id[32], scheduled_departure[64], seat_no[64];
    SQLLEN row_count;

    char result[512];

    trim_trailing(bookID); /* Remove white spaces from `bookID` */

    if (strlen(bookID) == 0) {
        write_error(msg_win, "`book Id` cannot be empty");
        return;
    }

    /* Check if booking with such booking ID exists */
    SQLBindParameter(booking_stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(bookID), 0, bookID, sizeof(bookID), NULL);
    SQLExecute(booking_stmt);

    SQLRowCount(booking_stmt, &row_count);
    SQLCloseCursor(booking_stmt);

    if (row_count > 0) {
        SQLBindParameter(created_boarding_passes_stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(bookID), 0, bookID, sizeof(bookID), NULL);
        SQLExecute(created_boarding_passes_stmt);

        SQLBindCol(created_boarding_passes_stmt, 1, SQL_C_CHAR, passenger_name, sizeof(passenger_name), NULL);
        SQLBindCol(created_boarding_passes_stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
        SQLBindCol(created_boarding_passes_stmt, 3, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
        SQLBindCol(created_boarding_passes_stmt, 4, SQL_C_CHAR, seat_no, sizeof(seat_no), NULL);

        /* Fetch and process the results */
        *n_choices = 0;
        while (SQL_SUCCEEDED(SQLFetch(created_boarding_passes_stmt))) {
            if (*n_choices < max_rows) {
                /* Allocate memory for the current choice */
                (*choices)[*n_choices] = (char*)malloc(max_length * sizeof(char));

                passenger_name[20] = '\0'; /* truncate the passenger name */

                /* Capture and format result */
                sprintf(result, "(%d) PN: %s, FID: %s, SD: %s, SNO: %s", (*n_choices)+1, passenger_name, flight_id, scheduled_departure, seat_no);

                /* Use proper indexing and dereferencing for choices */
                write_choice(result, choices, (*n_choices), max_length);
                (*n_choices)++;
            }
        }

        SQLCloseCursor(created_boarding_passes_stmt);

        if (*n_choices == 0) {
            write_error(msg_win, "booking already has all boarding passes");
        } else {
            write_success(msg_win, "PN: Passenger Name, FID: Flight ID, SD: Scheduled Departure, SNO: Seat No");
        }
    } else {
        write_error(msg_win, "no booking found with given `book ID`");
    }
}
