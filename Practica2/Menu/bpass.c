/*
* Created by roberto on 3/5/21.
*/
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <stdio.h>
#include "lbpass.h"
#include "odbc.h"
#include "utils.h"

void    results_bpass(char * bookID,
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows,
                       char *errMsg)
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
    SQLCHAR passenger_name[32];
    SQLCHAR flight_id[32];
    SQLCHAR scheduled_departure[64];
    SQLCHAR seat_no[64];
    char query[4000];
    char result[512];
    int i = 0; /* Counter for created boarding passess */

    trim_trailing(bookID); /* Remove white spaces from `bookID` */

    if (strlen(bookID) == 0) {
        sprintf(errMsg, "`book Id` cannot be empty");
        return;
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        sprintf(errMsg, "could not connect to database");
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* SQL statement to execute the DO block with parameters */
    sprintf(query, "DO $$ \
        DECLARE \
            ticket_flight_without_boarding_pass RECORD; \
            available_seat_no CHAR VARYING(4); \
            last_boarding_no INT; \
        BEGIN \
            IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'results') THEN \
                DROP TABLE results; \
            END IF; \
            CREATE TABLE results ( \
                passenger_name TEXT, \
                flight_id INT, \
                scheduled_departure TIMESTAMP WITH TIME ZONE, \
                seat_no CHAR VARYING(4) \
            ); \
            FOR ticket_flight_without_boarding_pass IN ( \
                SELECT \
                    tf.flight_id, \
                    tf.ticket_no, \
                    t.passenger_name \
                FROM \
                    ticket_flights tf \
                    NATURAL JOIN tickets t \
                    LEFT JOIN boarding_passes bp \
                    ON bp.flight_id = tf.flight_id AND bp.ticket_no = tf.ticket_no \
                WHERE \
                    bp.flight_id IS NULL AND bp.ticket_no IS NULL AND t.book_ref = '%s' \
                ORDER BY tf.ticket_no ASC \
            ) LOOP \
                SELECT s.seat_no \
                FROM seats s \
                WHERE s.aircraft_code IN ( \
                    SELECT f.aircraft_code \
                    FROM flights f \
                    WHERE f.flight_id = ticket_flight_without_boarding_pass.flight_id \
                ) \
                AND NOT EXISTS( \
                    SELECT 1 \
                    FROM boarding_passes bp \
                    WHERE bp.flight_id = ticket_flight_without_boarding_pass.flight_id \
                    AND bp.seat_no = s.seat_no \
                ) \
                ORDER BY s.seat_no ASC, s.aircraft_code ASC \
                LIMIT 1 INTO available_seat_no; \
                SELECT COALESCE(MAX(boarding_no), 0) \
                INTO last_boarding_no \
                FROM boarding_passes \
                WHERE flight_id = ticket_flight_without_boarding_pass.flight_id; \
                INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no) \
                VALUES ( \
                    ticket_flight_without_boarding_pass.ticket_no, \
                    ticket_flight_without_boarding_pass.flight_id, \
                    last_boarding_no + 1, \
                    available_seat_no \
                ); \
                INSERT INTO results (passenger_name, flight_id, scheduled_departure, seat_no) \
                    SELECT \
                        passenger_name, \
                        flight_id, \
                        scheduled_departure, \
                        seat_no \
                    FROM \
                        tickets \
                        NATURAL JOIN ticket_flights \
                        NATURAL JOIN flights \
                        NATURAL JOIN boarding_passes \
                    WHERE \
                        ticket_no = ticket_flight_without_boarding_pass.ticket_no AND flight_id = ticket_flight_without_boarding_pass.flight_id \
                    LIMIT 1; \
            END LOOP; \
        END; \
        $$;", bookID);

    ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query, "SELECT * from results");

    ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_CHAR, passenger_name, sizeof(passenger_name), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, flight_id, sizeof(flight_id), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, seat_no, sizeof(seat_no), NULL);

    /* Fetch and process the results */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (i < max_rows) {
            /* Allocate memory for the current i */
            (*choices)[i] = (char*)malloc(max_length * sizeof(char));

            if ((*choices)[i] == NULL) {
                break;
            }

            passenger_name[20] = '\0'; /* truncate the passenger name */

            /* Capture and format result */
            sprintf(result, "(%d) PN: %s, FID: %s, SD: %s, SNO: %s", i+1, passenger_name, flight_id, scheduled_departure, seat_no);

            /* Use proper indexing and dereferencing for choices */
            write_choice(result, choices, i, max_length);
            i++;
        }
    }

    SQLCloseCursor(stmt);

    /* Update the number of choices */
    *n_choices = i;

    if (i == 0) {
        sprintf(errMsg, "booking already has all boarding passes");
    }

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return ;
    }
}
