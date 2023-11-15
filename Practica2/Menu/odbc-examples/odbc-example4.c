#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 4 with a queries build on-the-fly, the good way
 */

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



int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLCHAR x;
    SQLCHAR y[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*)CREATE_BOARDING_PASSES_FUNCTION, SQL_NTS);
    SQLExecute(stmt);

    SQLPrepare(stmt, (SQLCHAR*)"SELECT * FROM create_boarding_passes(?)", SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, &x, 0, NULL);

    printf("x = ");
    fflush(stdout);
    while (scanf("%s", &x) != EOF) {
        printf("x = %s\n", &x);
        SQLExecute(stmt);
        SQLCHAR sqlstate[6], message[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER native_error;
        SQLSMALLINT msg_length;

        SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, sqlstate, &native_error, message, SQL_MAX_MESSAGE_LENGTH, &msg_length);

        printf("SQLSTATE: %s\n", sqlstate);
        printf("Native Error: %d\n", native_error);
        printf("Message: %s\n", message);

        
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

