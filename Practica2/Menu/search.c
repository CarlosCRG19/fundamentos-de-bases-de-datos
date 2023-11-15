#include <sql.h>
#include <sqltypes.h>
#include "search.h"
#include "odbc.h"
#include "utils.h"

void results_search(char * from, char * to, char * date, int * n_choices,
                    char *** choices, int max_length, int max_rows, WINDOW *msg_win)
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
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    SQLCHAR scheduled_departure[64], scheduled_arrival[64];
    SQLINTEGER n_connections, n_availabe_seats;
    SQLLEN row_count;

    char result[512];
    char query[4000];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        write_error(msg_win, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* Use sprintf to insert values into the query string */
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
            "        f.departure_airport = '%s'\n"
            "        AND f.arrival_airport = '%s'\n"
            "        AND DATE(f.scheduled_departure) = '%s'\n"
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
            "        AND f1.departure_airport = '%s'\n"
            "        AND f2.arrival_airport = '%s'\n"
            "        AND DATE(f1.scheduled_departure) = '%s'\n"
            "    GROUP BY f1.departure_airport, f2.arrival_airport, f1.scheduled_departure, f2.scheduled_arrival\n"
            ") AS combined_result\n"
            "WHERE seats_available > 0\n"
            "ORDER BY scheduled_arrival - scheduled_departure ASC;",
            from, to, date, from, to, date);

    SQLExecDirect(stmt, (SQLCHAR *)query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);
    SQLBindCol(stmt, 3, SQL_INTEGER, &n_connections, sizeof(n_connections), NULL);
    SQLBindCol(stmt, 4, SQL_INTEGER, &n_availabe_seats, sizeof(n_connections), NULL);

    /* Fetch and process the results */
    *n_choices = 0;
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (*n_choices < max_rows) {
            /* Allocate memory for the current choice */
            (*choices)[*n_choices] = (char*)malloc(max_length * sizeof(char));

            /* Capture and format result */
            sprintf(result, "(%d) SD: %s, SA: %s, NC: %d, AS: %d", (*n_choices)+1, scheduled_departure, scheduled_arrival, n_connections, n_availabe_seats);

            /* Use proper indexing and dereferencing for choices */
            write_choice(result, choices, (*n_choices), max_length);
            (*n_choices)++;
        }
    }

    SQLCloseCursor(stmt);

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return ;
    }
}
