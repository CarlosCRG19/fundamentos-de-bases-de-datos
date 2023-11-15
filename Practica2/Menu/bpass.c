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

const char *BOOKINGS_QUERY = "SELECT 1 FROM bookings WHERE book_ref = ? LIMIT 1;";
const char* CREATE_BOARDING_PASSES_FUNCTION= "CREATE OR REPLACE FUNCTION create_boarding_passes(book_ref_param TEXT) "
                                          "RETURNS TABLE ( "
                                          "    passenger_name TEXT, "
                                          "    flight_id INT, "
                                          "    scheduled_departure TIMESTAMP WITH TIME ZONE, "
                                          "    seat_no CHAR VARYING(4) "
                                          ") "
                                          "AS $$ "
                                          "DECLARE "
                                          "    ticket_flight_without_boarding_pass RECORD; "
                                          "    available_seat_no CHAR VARYING(4); "
                                          "    last_boarding_no INT; "
                                          "BEGIN "
                                          "    -- drop the results table if it exists "
                                          "    IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'results') THEN "
                                          "        DROP TABLE results; "
                                          "    END IF; "
                                          " "
                                          "    -- temporary table to store the data of created boarding passes "
                                          "    CREATE TEMPORARY TABLE results ( "
                                          "        passenger_name TEXT, "
                                          "        flight_id INT, "
                                          "        scheduled_departure TIMESTAMP WITH TIME ZONE, "
                                          "        seat_no CHAR VARYING(4) "
                                          "    ); "
                                          " "
                                          "    -- main iteration "
                                          "    FOR ticket_flight_without_boarding_pass IN ( "
                                          "        -- query to get all flight tickets that do "
                                          "        -- not have an associated boarding pass "
                                          "        SELECT "
                                          "            tf.flight_id, "
                                          "            tf.ticket_no, "
                                          "            t.passenger_name "
                                          "        FROM "
                                          "            ticket_flights tf "
                                          "            NATURAL JOIN tickets t "
                                          "            LEFT JOIN boarding_passes bp "
                                          "            ON bp.flight_id = tf.flight_id AND bp.ticket_no = tf.ticket_no "
                                          "        WHERE "
                                          "            bp.flight_id IS NULL AND bp.ticket_no IS NULL AND t.book_ref = book_ref_param "
                                          "        ORDER BY tf.ticket_no ASC -- order according to instructions "
                                          "    ) LOOP "
                                          "        -- query to get the first available seat on the flight "
                                          "        SELECT s.seat_no "
                                          "        FROM seats s "
                                          "        WHERE s.aircraft_code IN ( "
                                          "            SELECT f.aircraft_code "
                                          "            FROM flights f "
                                          "            WHERE f.flight_id = ticket_flight_without_boarding_pass.flight_id "
                                          "        ) "
                                          "        AND NOT EXISTS( "
                                          "            -- exclude all seats that are already assigned "
                                          "            SELECT 1 "
                                          "            FROM boarding_passes bp "
                                          "            WHERE bp.flight_id = ticket_flight_without_boarding_pass.flight_id "
                                          "            AND bp.seat_no = s.seat_no "
                                          "        ) "
                                          "        ORDER BY s.seat_no ASC, s.aircraft_code ASC "
                                          "        LIMIT 1 INTO available_seat_no; "
                                          " "
                                          "        -- query to get the last boarding pass number "
                                          "        -- associated with the flight "
                                          "        SELECT COALESCE(MAX(boarding_no), 0) "
                                          "        INTO last_boarding_no "
                                          "        FROM boarding_passes bp "
                                          "        WHERE bp.flight_id = ticket_flight_without_boarding_pass.flight_id; "
                                          " "
                                          "        -- create new boarding pass assigning the found "
                                          "        -- available seat to the ticket flight "
                                          "        INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no) "
                                          "        VALUES ( "
                                          "            ticket_flight_without_boarding_pass.ticket_no, "
                                          "            ticket_flight_without_boarding_pass.flight_id, "
                                          "            last_boarding_no + 1, "
                                          "            available_seat_no "
                                          "        ); "
                                          " "
                                          "        -- save created boarding pass into results table "
                                          "        INSERT INTO results (passenger_name, flight_id, scheduled_departure, seat_no) "
                                          "            SELECT "
                                          "              t.passenger_name, "
                                          "              tf.flight_id, "
                                          "              f.scheduled_departure, "
                                          "              bp.seat_no "
                                          "            FROM "
                                          "              tickets t "
                                          "              NATURAL JOIN ticket_flights tf "
                                          "              NATURAL JOIN flights f "
                                          "              NATURAL JOIN boarding_passes bp "
                                          "            WHERE "
                                          "              tf.ticket_no = ticket_flight_without_boarding_pass.ticket_no AND tf.flight_id = ticket_flight_without_boarding_pass.flight_id "
                                          "            LIMIT 1; "
                                          "    END LOOP; "
                                          " "
                                          "    -- return the contents of the results table "
                                          "    RETURN QUERY SELECT * FROM results; "
                                          "END; "
                                          "$$ LANGUAGE plpgsql;";
const char * RESULTS_QUERY = "SELECT * FROM create_boarding_passes(?);";

void    results_bpass(char * bookID,
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
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    SQLCHAR passenger_name[32], flight_id[32], scheduled_departure[64], seat_no[64];
    SQLLEN row_count;

    char result[512];

    trim_trailing(bookID); /* Remove white spaces from `bookID` */

    if (strlen(bookID) == 0) {
        write_error(msg_win, "`book Id` cannot be empty");
        return;
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        write_error(msg_win, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* Check if booking with such booking ID exists */
    SQLPrepare(stmt, (SQLCHAR *)BOOKINGS_QUERY, SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(bookID), 0, bookID, sizeof(bookID), NULL);
    SQLExecute(stmt);

    SQLRowCount(stmt, &row_count);
    SQLCloseCursor(stmt);

    if (row_count > 0) {
        /* SQL statement to execute the DO block with parameters */
        SQLPrepare(stmt, (SQLCHAR *)CREATE_BOARDING_PASSES_FUNCTION, SQL_NTS);
        SQLExecute(stmt);
        SQLCloseCursor(stmt);

        SQLPrepare(stmt, (SQLCHAR *)RESULTS_QUERY, SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(bookID), 0, bookID, sizeof(bookID), NULL);
        SQLExecute(stmt);

        SQLBindCol(stmt, 1, SQL_C_CHAR, passenger_name, sizeof(passenger_name), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, seat_no, sizeof(seat_no), NULL);

        /* Fetch and process the results */
        *n_choices = 0;
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
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

        SQLCloseCursor(stmt);

        if (*n_choices == 0) {
            write_error(msg_win, "booking already has all boarding passes");
        } else {
            write_success(msg_win, "PN: Passenger Name, FID: Flight ID, SD: Scheduled Departure, SNO: Seat No");
        }
    } else {
        write_error(msg_win, "no booking found with given `book ID`");
    }

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return ;
    }
}
