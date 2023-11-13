/*
* Created by roberto on 3/5/21.
*/
#include <stdlib.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "search.h"

void    results_search(char *from, char *to, char *date,
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows)
   /**here you need to do your query and fill the choices array of strings
 *
 * @param from form field from
 * @param to form field to
 * @param date form field date
 * @param n_choices fill this with the number of results
 * @param choices fill this with the actual results
 * @param max_length output win maximum width
 * @param max_rows output win maximum number of rows
  */
{
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
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

        int i=0;
        int t=0;
        /* 10 commandments from King Jorge Bible */

        /* Loop through the rows in the result-set */
        /*while (SQL_SUCCEEDED(ret = SQLFetch(stmt)) && i < max_rows) {*/
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            (*choices)[i] = malloc(max_length * sizeof(char));
            snprintf((*choices)[i], max_length, "Departure: %s, Arrival: %s, Scheduled Departure: %s, Scheduled Arrival: %s",
                     departure, arrival, scheduledDeparture, scheduledArrival);
            i++;
        } 

        *n_choices = i;
    } else {
        printf("Error ejecutando query");
    }

    /* Free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    
    /* Disconnect from the database */
    ret = odbc_disconnect(NULL, dbc); /* Assuming odbc_disconnect does not need the environment handle */
    if (!SQL_SUCCEEDED(ret)) {
        return;
    }
}


