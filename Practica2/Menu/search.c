#include <sql.h>
#include <sqltypes.h>
#include "search.h"
#include "odbc.h"
#include "utils.h"

const char *QUERY ="SELECT * FROM "
                   "( "
                   "    SELECT "
                   "        f.scheduled_departure AS scheduled_departure, "
                   "        f.scheduled_arrival AS scheduled_arrival, "
                   "        0 AS no_connections, "
                   "        COUNT(s.seat_no) AS seats_available, "
                   "        f.flight_id AS flight_id_1, "
                   "        -1 AS flight_id_2 "
                   "    FROM flights f "
                   "    JOIN seats s ON f.aircraft_code = s.aircraft_code "
                   "    LEFT JOIN boarding_passes bp ON f.flight_id = bp.flight_id AND s.seat_no = bp.seat_no "
                   "    WHERE "
                   "        f.departure_airport = ? "
                   "        AND f.arrival_airport = ? "
                   "        AND DATE(f.scheduled_departure) = ? "
                   "        AND bp.ticket_no IS NULL "
                   "    GROUP BY f.flight_id "
                   " "
                   "    UNION "
                   " "
                   "    SELECT "
                   "        f1.scheduled_departure AS scheduled_departure, "
                   "        f2.scheduled_arrival AS scheduled_arrival, "
                   "        1 AS no_connections, "
                   "        LEAST(COUNT(DISTINCT s1.seat_no), COUNT(DISTINCT s2.seat_no)) AS seats_available, "
                   "        f1.flight_id AS flight_id_1, "
                   "        f2.flight_id AS flight_id_2 "
                   "    FROM flights f1 "
                   "    JOIN flights f2 ON f1.arrival_airport = f2.departure_airport "
                   "    JOIN seats s1 ON f1.aircraft_code = s1.aircraft_code "
                   "    JOIN seats s2 ON f2.aircraft_code = s2.aircraft_code "
                   "    WHERE s1.seat_no NOT IN ( "
                   "            SELECT seat_no "
                   "            FROM boarding_passes "
                   "            WHERE flight_id = f1.flight_id) "
                   "        AND s2.seat_no NOT IN ( "
                   "            SELECT seat_no "
                   "            FROM boarding_passes "
                   "            WHERE flight_id = f2.flight_id)  "
                   "        AND f1.scheduled_arrival < f2.scheduled_departure "
                   "        AND EXTRACT(EPOCH FROM (f2.scheduled_departure - f1.scheduled_arrival)) / 3600 <= 24 "
                   "        AND f1.departure_airport = ? "
                   "        AND f2.arrival_airport = ? "
                   "        AND DATE(f1.scheduled_departure) = ? "
                   "    GROUP BY f1.departure_airport, f2.arrival_airport, f1.scheduled_departure, f2.scheduled_arrival, f1.flight_id, f2.flight_id "
                   ") AS combined_result "
                   "WHERE seats_available > 0 "
                   "ORDER BY scheduled_arrival - scheduled_departure ASC;";

void results_search(char * from, char * to, char * date, int * n_choices,
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
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    SQLCHAR scheduled_departure[64], scheduled_arrival[64];
    SQLINTEGER n_connections, n_availabe_seats, flight_id_1, flight_id_2;
    SQLLEN row_count;

    char result[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        write_error(msg_win, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR *)QUERY, SQL_NTS);

    /* Bind parameters */
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(from), 0, from, sizeof(from), NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(to), 0, to, sizeof(to), NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(date), 0, date, sizeof(date), NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(from), 0, from, sizeof(from), NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(to), 0, to, sizeof(to), NULL);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(date), 0, date, sizeof(date), NULL);

    SQLExecute(stmt);

    SQLBindCol(stmt, 1, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);
    SQLBindCol(stmt, 3, SQL_INTEGER, &n_connections, sizeof(n_connections), NULL);
    SQLBindCol(stmt, 4, SQL_INTEGER, &n_availabe_seats, sizeof(n_availabe_seats), NULL);
    SQLBindCol(stmt, 5, SQL_INTEGER, &flight_id_1, sizeof(flight_id_1), NULL);
    SQLBindCol(stmt, 6, SQL_INTEGER, &flight_id_2, sizeof(flight_id_2), NULL);

    /* Fetch and process the results */
    *n_choices = 0;
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
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

    SQLCloseCursor(stmt);

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return ;
    }
}

void flight_details(char * flight_id, WINDOW *msg_win) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    SQLCHAR aircraft_code[8], scheduled_departure[64], scheduled_arrival[64];

    char result[512];
    char query[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        write_error(msg_win, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /*SQLPrepare(stmt, (SQLCHAR *) "SELECT aircraft_code, scheduled_departure, scheduled_arrival \
            FROM flights f \
            WHERE f.flight_id = ? LIMIT 1;", SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &flight_id, 0, NULL);

    SQLExecute(stmt);*/
    sprintf(query, "SELECT aircraft_code, scheduled_departure, scheduled_arrival FROM flights WHERE flight_id = %s", flight_id);
    SQLExecDirect(stmt, (SQLCHAR *)query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_CHAR, aircraft_code, sizeof(aircraft_code), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);

    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        sprintf(result, "ID: %s, Aircraft Code: %s, Departure: %s, Arrival: %s", flight_id, aircraft_code, scheduled_departure, scheduled_arrival);
        write_success(msg_win, result);
    } else {
        write_error(msg_win, "error executing query");
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
