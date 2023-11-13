#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 3 with a queries build on-the-fly, the bad way
 */

int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char x[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    char *from = "VKO";
    char *to = "HMA";
    char *date = "2017/08/26";

    printf("x = ");
    fflush(stdout);
    while (fgets(x, sizeof(x), stdin) != NULL) {
        char query[512];
        snprintf(query, sizeof(query), "SELECT a1.airport_code AS departure_airport, "
             "a2.airport_code AS arrival_airport, scheduled_departure, scheduled_arrival "
             "FROM flights "
             "JOIN airports_data a1 ON departure_airport = a1.airport_code "
             "JOIN airports_data a2 ON arrival_airport = a2.airport_code "
             "WHERE departure_airport = '%s' AND arrival_airport = '%s' AND DATE(scheduled_departure) = '%s';",
             from, to, date);
        printf("%s\n", query); 

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            SQLCHAR departure[64], arrival[64], scheduledDeparture[64], scheduledArrival[64];

            SQLBindCol(stmt, 1, SQL_C_CHAR, departure, sizeof(departure), NULL);
            SQLBindCol(stmt, 2, SQL_C_CHAR, arrival, sizeof(arrival), NULL);
            SQLBindCol(stmt, 3, SQL_C_CHAR, scheduledDeparture, sizeof(scheduledDeparture), NULL);
            SQLBindCol(stmt, 4, SQL_C_CHAR, scheduledArrival, sizeof(scheduledArrival), NULL);

        } else {
            printf("Error executing query\n");
        }

        SQLCloseCursor(stmt);

        printf("x = ");
        fflush(stdout);
    }
    printf("\n");
    
    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

