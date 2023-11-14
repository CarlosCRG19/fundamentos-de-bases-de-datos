/*
* Created by roberto on 3/5/21.
*/
#include <stdlib.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "search.h"
#include "utils.h"

#define MAX_ERROR_LENGTH 256


void handle_error(int *n_choices, char ***choices, const char *error_message) {
    *n_choices = 1;
    *choices = malloc(sizeof(char *));
    (*choices)[0] = malloc(MAX_ERROR_LENGTH * sizeof(char));
    snprintf((*choices)[0], MAX_ERROR_LENGTH, "ERROR: %s", error_message);
}

void free_choices(char ***choices, int n_choices) {
    int i;
    for (i = 0; i < n_choices; i++) {
        free((*choices)[i]);
    }
    free(*choices);
}

void results_search(char *from, char *to, char *date,
                    int *n_choices, char ***choices,
                    int max_length, int max_rows) {
    if (*n_choices > 0) {
        free_choices(choices, *n_choices);
        *n_choices = 0;
    }

    if (is_empty(from) || is_empty(to) || is_empty(date)) {
        char missing_fields[MAX_ERROR_LENGTH];
        snprintf(missing_fields, sizeof(missing_fields), "missing fields %s%s%s",
                 (is_empty(from) ? "from" : ""),
                 (is_empty(to) ? (is_empty(from) ? ", to" : "to") : ""),
                 (is_empty(date) ? (is_empty(from) || is_empty(to) ? ", date" : "date") : ""));

        handle_error(n_choices, choices, missing_fields);
        return;
    }

    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        handle_error(n_choices, choices, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    char query[512];
    snprintf(query, sizeof(query), "SELECT a1.airport_code AS departure_airport, "
                                   "a2.airport_code AS arrival_airport, scheduled_departure, scheduled_arrival "
                                   "FROM flights "
                                   "JOIN airports_data a1 ON departure_airport = a1.airport_code "
                                   "JOIN airports_data a2 ON arrival_airport = a2.airport_code "
                                   "WHERE departure_airport = '%s' AND arrival_airport = '%s' AND DATE(scheduled_departure) = '%s';",
             from, to, date);

    /* Execute the SQL query */
    ret = SQLExecDirect(stmt, (SQLCHAR *)query, SQL_NTS);

    if (SQL_SUCCEEDED(ret)) {
        SQLCHAR departure[64], arrival[64], scheduledDeparture[64], scheduledArrival[64];

        SQLBindCol(stmt, 1, SQL_C_CHAR, departure, sizeof(departure), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, arrival, sizeof(arrival), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, scheduledDeparture, sizeof(scheduledDeparture), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, scheduledArrival, sizeof(scheduledArrival), NULL);

        int i = 0;

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt)) && i < max_rows) {
            (*choices)[i] = malloc(max_length * sizeof(char));
            snprintf((*choices)[i], max_length, "Departure: %s, Arrival: %s, Scheduled Departure: %s, Scheduled Arrival: %s",
                     departure, arrival, scheduledDeparture, scheduledArrival);
            i++;
        }

        *n_choices = i;
    } else {
        handle_error(n_choices, choices, "query execution failed");
    }

    /* Free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* Disconnect from the database */
    ret = odbc_disconnect(NULL, dbc); /* Assuming odbc_disconnect does not need the environment handle */
    if (!SQL_SUCCEEDED(ret)) {
        handle_error(n_choices, choices, "could not disconnect from database");
        return;
    }
}

