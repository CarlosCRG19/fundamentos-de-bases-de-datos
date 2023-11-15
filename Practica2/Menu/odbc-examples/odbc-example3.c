#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 3 with a queries build on-the-fly, the bad way
 */

/**
 * Remove trailing white space characters from string
 */
void trim_trailing(char * str)
{
    int index, i;

    /* Set default index */
    index = -1;

    /* Find last index of non-white space character */
    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index= i;
        }

        i++;
    }

    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}

int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char x[512];
    SQLCHAR y[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    printf("x = ");
    fflush(stdout);
    while (fgets(x, sizeof(x), stdin) != NULL) {
        char query[3500];
        trim_trailing(x);
        sprintf(query, "SELECT * FROM\n"
        "(\n"
        "    SELECT\n"
        "        f.scheduled_departure AS scheduled_departure,\n"
        "        f.scheduled_arrival AS scheduled_arrival,\n"
        "        0 AS no_connections,\n"
        "        COUNT(s.seat_no) AS seats_available\n"
        "    FROM flights f\n"
        "    JOIN seats s ON f.aircraft_code = s.aircraft_code\n"
        "    LEFT JOIN boarding_passes bp ON f.flight_id = bp.flight_id AND s.seat_no = bp.seat_no\n"
        "    WHERE\n"
        "        f.departure_airport = 'REN'\n"
        "        AND f.arrival_airport = 'VKO'\n"
        "        AND DATE(f.scheduled_departure) = '2017-08-07'\n"
        "        AND bp.ticket_no IS NULL\n"
        "    GROUP BY f.flight_id\n"
        "\n"
        "    UNION\n"
        "\n"
        "    SELECT\n"
        "        f1.scheduled_departure AS scheduled_departure,\n"
        "        f2.scheduled_arrival AS scheduled_arrival,\n"
        "        1 AS no_connections,\n"
        "        LEAST(COUNT(DISTINCT s1.seat_no), COUNT(DISTINCT s2.seat_no)) AS seats_available\n"
        "    FROM flights f1\n"
        "    JOIN flights f2 ON f1.arrival_airport = f2.departure_airport\n"
        "    JOIN seats s1 ON f1.aircraft_code = s1.aircraft_code\n"
        "    JOIN seats s2 ON f2.aircraft_code = s2.aircraft_code\n"
        "    WHERE s1.seat_no NOT IN (\n"
        "            SELECT seat_no\n"
        "            FROM boarding_passes\n"
        "            WHERE flight_id = f1.flight_id)\n"
        "        AND s2.seat_no NOT IN (\n"
        "            SELECT seat_no\n"
        "            FROM boarding_passes\n"
        "            WHERE flight_id = f2.flight_id) \n"
        "        AND f1.scheduled_arrival < f2.scheduled_departure\n"
        "        AND EXTRACT(EPOCH FROM (f2.scheduled_departure - f1.scheduled_arrival)) / 3600 <= 24\n"
        "        AND f1.departure_airport = 'REN'\n"
        "        AND f2.arrival_airport = 'VKO'\n"
        "        AND DATE(f1.scheduled_departure) = '2017-08-07'\n"
        "    GROUP BY f1.departure_airport, f2.arrival_airport, f1.scheduled_departure, f2.scheduled_arrival\n"
        ") AS combined_result\n"
        "WHERE seats_available > 0\n"
        "ORDER BY scheduled_arrival - scheduled_departure ASC;");
        printf("%s", query); 

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        SQLBindCol(stmt, 1, SQL_C_CHAR, y, sizeof(y), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("y = %s\n", y);
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
