/*
* Created by roberto on 3/5/21.
*/
#include "lbpass.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
/*
 * example 3 with queries built on-the-fly, the bad way
 */

void results_bpass(char * bookID,
                   int * n_choices, char * choices,
                   int max_length,
                   int max_rows)
{
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret;
    char psg[512];
    char fid[512];
    char sch[512];
    char seat[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    char query[3000];
    sprintf(query, "do $$ DECLARE new_tickets RECORD; flightX RECORD; boarding_num INT; selected_seat character varying(4); begin IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'temp_tickets') THEN DROP TABLE temp_tickets; END IF; CREATE TABLE temp_tickets AS ( SELECT passenger_name, flight_id, scheduled_departure, seat_no FROM tickets NATURAL JOIN ticket_flights NATURAL JOIN flights NATURAL JOIN boarding_passes WHERE ticket_no = '' AND flight_id = -1 ); FOR flightX IN ( SELECT tickets_book.flight_id as flights_id_no_bp, tickets_book.ticket_no as temp_ticket_no FROM (ticket_flights NATURAL JOIN tickets) as tickets_book LEFT JOIN boarding_passes ON tickets_book.ticket_no = boarding_passes.ticket_no and tickets_book.flight_id = boarding_passes.flight_id WHERE (boarding_passes.ticket_no IS NULL and boarding_passes.flight_id IS NULL) and book_ref = '%s') LOOP SELECT total_seats as empty_seat FROM ( SELECT flight_id, aircraft_code, seat_no as total_seats FROM flights NATURAL JOIN aircrafts_data NATURAL JOIN seats WHERE flight_id = flightX.flights_id_no_bp ) WHERE total_seats NOT IN ( SELECT seat_no as booked_seats FROM boarding_passes WHERE flight_id = flightX.flights_id_no_bp ) ORDER BY aircraft_code ASC, empty_seat ASC LIMIT 1 INTO selected_seat; SELECT boarding_no FROM boarding_passes WHERE flight_id = flightX.flights_id_no_bp ORDER BY boarding_no DESC LIMIT 1 INTO boarding_num; IF boarding_num IS NULL THEN INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no) VALUES (flightX.temp_ticket_no, flightX.flights_id_no_bp, 1, selected_seat); ELSE INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no) VALUES (flightX.temp_ticket_no, flightX.flights_id_no_bp, boarding_num + 1, selected_seat); END IF; INSERT INTO temp_tickets (passenger_name, flight_id, scheduled_departure, seat_no) SELECT passenger_name, flight_id, scheduled_departure, seat_no FROM tickets NATURAL JOIN ticket_flights NATURAL JOIN flights NATURAL JOIN boarding_passes WHERE ticket_no = flightX.temp_ticket_no AND flight_id = flightX.flights_id_no_bp LIMIT 1; END LOOP; end; $$;", bookID);

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLCloseCursor(stmt);

    char query2[200];
    sprintf(query2, "SELECT * FROM temp_tickets;");

    SQLExecDirect(stmt, (SQLCHAR*) query2, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_CHAR, psg, sizeof(psg), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, fid, sizeof(fid), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, sch, sizeof(sch), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, seat, sizeof(seat), NULL);

    *n_choices = 0; // Reset the count
    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (*n_choices < max_rows) {
            // Allocate memory for each choice
            (*choices)[*n_choices] = (char *)malloc(max_length * sizeof(char));
            
            // Copy data to choices array
            snprintf((*choices)[*n_choices], max_length, "%s    | %s    | %s    | %s", psg, fid, sch, seat);
            (*n_choices)++;
        }
    }

    SQLCloseCursor(stmt);

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
