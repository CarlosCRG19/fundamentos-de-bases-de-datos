#include <sql.h>
#include <sqltypes.h>
#include <string.h>
#include "search.h"
#include "odbc.h"
#include "utils.h"

void results_search(SQLHSTMT stmt, char * from, char * to, char * date, int * n_choices,
                    char *** choices, int max_length, int max_rows, WINDOW *msg_win, char ** search_flight_ids_1, char ** search_flight_ids_2)
   /**here you need to do your query and fill the choices array of strings
 *
 * @param from form field from
 * @param to form field to
 * @param n_choices fill this with the number of results
 * @param choices fill this with the actual results
 * @param max_length output win maximum width
 * @param max_rows output win maximum number of rows
  */
{
    SQLCHAR scheduled_departure[64], scheduled_arrival[64];
    SQLINTEGER n_connections, n_availabe_seats, flight_id_1, flight_id_2;
    SQLLEN row_count;

    char result[512], missing_fields[124] = "the following fields are missing: ";

    trim_trailing(from);
    trim_trailing(to);
    trim_trailing(date);

    /* Check if any field is missing */
    if (strlen(from) == 0 || strlen(to) == 0 || strlen(date) == 0) {
        if (strlen(from) == 0) {
            strcat(missing_fields, " `from`");
        }

        if (strlen(to) == 0) {
            strcat(missing_fields, " `to`");
        }

        if (strlen(date) == 0) {
            strcat(missing_fields, " `date`");
        }

        write_error(msg_win, missing_fields);
        return;
    }

    /* Bind parameters */
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(from), 0, from, sizeof(from), NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(to), 0, to, sizeof(to), NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(date), 0, date, sizeof(date), NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(from), 0, from, sizeof(from), NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(to), 0, to, sizeof(to), NULL);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(date), 0, date, sizeof(date), NULL);

    if (SQL_SUCCEEDED(SQLExecute(stmt))) {
        SQLBindCol(stmt, 1, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);
        SQLBindCol(stmt, 3, SQL_INTEGER, &n_connections, sizeof(n_connections), NULL);
        SQLBindCol(stmt, 4, SQL_INTEGER, &n_availabe_seats, sizeof(n_availabe_seats), NULL);
        SQLBindCol(stmt, 5, SQL_INTEGER, &flight_id_1, sizeof(flight_id_1), NULL);
        SQLBindCol(stmt, 6, SQL_INTEGER, &flight_id_2, sizeof(flight_id_2), NULL);

        /* Fetch and process the results */
        *n_choices = 0;
        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            if (*n_choices < max_rows) {
                /* Allocate memory for the current choice */
                (*choices)[*n_choices] = (char*)malloc(max_length * sizeof(char));
                search_flight_ids_1[*n_choices] = (char*)malloc(sizeof(flight_id_1));

                /* Capture and format result */
                sprintf(result, "(%d) SD: %s, SA: %s, NC: %d, AS: %d", (*n_choices)+1, scheduled_departure, scheduled_arrival, n_connections, n_availabe_seats);

                /* Use proper indexing and dereferencing for choices */
                write_choice(result, choices, (*n_choices), max_length);
                sprintf(search_flight_ids_1[*n_choices], "%d", flight_id_1);

                if (flight_id_2 != SQL_NULL_DATA) {
                    search_flight_ids_2[*n_choices] = (char*)malloc(sizeof(flight_id_2));
                    sprintf(search_flight_ids_2[*n_choices], "%d", flight_id_2);
                }

                (*n_choices)++;
            }
        }

        if (*n_choices == 0) {
            write_error(msg_win, "no flights found for the given data");
        }
    } else {
        write_error(msg_win, "error executing query");
    }

    SQLCloseCursor(stmt);
}

void flight_details(SQLHSTMT stmt, char *flight_id_1, char *flight_id_2, WINDOW *msg_win) {
    SQLCHAR aircraft_code[8], scheduled_departure[64], scheduled_arrival[64];
    char result[1024], temp[512];
    int i = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, flight_id_1, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, (flight_id_2[0] != '\0' ? flight_id_2 : "-1"), 0, NULL);

    SQLExecute(stmt);

    SQLBindCol(stmt, 1, SQL_C_CHAR, aircraft_code, sizeof(aircraft_code), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);

    result[0] = '\0';  
    while (SQL_SUCCEEDED(SQLFetch(stmt))) {
        sprintf(temp, "(Flight %d) FID: %s, AC: %s, SD: %s, SA: %s\n", i + 1, (i == 0) ? flight_id_1 : flight_id_2, aircraft_code, scheduled_departure, scheduled_arrival);
        strcat(result, temp);
        i++;
    }

    if (result[0] != '\0') {
        result[strlen(result) - 1] = '\0';
        write_msg(msg_win, result, -1, -1, "FLIGHTS DETAILS (FID: Flight ID, AC: Aircraft Code, SD: Scheduled Departure, SA: Scheduled Arrival)");
    } else {
        write_error(msg_win, "no results found");
    }

    SQLCloseCursor(stmt);
}
