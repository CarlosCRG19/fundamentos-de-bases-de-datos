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
        sprintf(query, "DO $$ \
            DECLARE \
                ticket_flight_without_boarding_pass RECORD; \
                available_seat_no CHAR VARYING(4); \
                last_boarding_no INT; \
            BEGIN \
                IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'results') THEN \
                    DROP TABLE results; \
                END IF; \
                CREATE TEMPORARY TABLE results ( \
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
            $$; \
            SELECT * FROM results;", x);
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
