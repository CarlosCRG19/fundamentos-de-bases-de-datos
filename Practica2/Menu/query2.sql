DO
$$
DECLARE
    ticket_flight_without_boarding_pass RECORD;
    available_seat_no CHAR VARYING(4);
    last_boarding_no INT;
BEGIN
    -- drop the results table if it exists
    IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'results') THEN
        DROP TABLE results;
    END IF;

    -- temporary table to store the data of created boarding passes
    CREATE TEMPORARY TABLE results (
        passenger_name TEXT,
        flight_id INT,
        scheduled_departure TIMESTAMP WITH TIME ZONE,
        seat_no CHAR VARYING(4)
    );

    -- main iteration
    FOR ticket_flight_without_boarding_pass IN (
        -- query to get all flight tickets that do
        -- not have an associated boarding pass
        SELECT
            tf.flight_id,
            tf.ticket_no,
            t.passenger_name
        FROM
            ticket_flights tf
            NATURAL JOIN tickets t
            LEFT JOIN boarding_passes bp
            ON bp.flight_id = tf.flight_id AND bp.ticket_no = tf.ticket_no
        WHERE
            bp.flight_id IS NULL AND bp.ticket_no IS NULL AND t.book_ref = 'D0B9CE' -- Replace book_ref_param with the specific value
        ORDER BY tf.ticket_no ASC -- order according to instructions
    ) LOOP
        -- query to get the first available seat on the flight
        SELECT s.seat_no
        FROM seats s
        WHERE s.aircraft_code IN (
            SELECT f.aircraft_code
            FROM flights f
            WHERE f.flight_id = ticket_flight_without_boarding_pass.flight_id
        )
        AND NOT EXISTS(
            -- exclude all seats that are already assigned
            SELECT 1
            FROM boarding_passes bp
            WHERE bp.flight_id = ticket_flight_without_boarding_pass.flight_id
            AND bp.seat_no = s.seat_no
        )
        ORDER BY s.seat_no ASC, s.aircraft_code ASC
        LIMIT 1 INTO available_seat_no;

        -- query to get the last boarding pass number
        -- associated with the flight
        SELECT COALESCE(MAX(boarding_no), 0)
        INTO last_boarding_no
        FROM boarding_passes
        WHERE flight_id = ticket_flight_without_boarding_pass.flight_id;

        -- create new boarding pass assigning the found
        -- available seat to the ticket flight
        INSERT INTO boarding_passes (ticket_no, flight_id, boarding_no, seat_no)
        VALUES (
            ticket_flight_without_boarding_pass.ticket_no,
            ticket_flight_without_boarding_pass.flight_id,
            last_boarding_no + 1,
            available_seat_no
        );

        -- save created boarding pass into results table
        INSERT INTO results (passenger_name, flight_id, scheduled_departure, seat_no)
            SELECT
              passenger_name,
              flight_id,
              scheduled_departure,
              seat_no
            FROM
              tickets
              NATURAL JOIN ticket_flights
              NATURAL JOIN flights
              NATURAL JOIN boarding_passes
            WHERE
              ticket_no = ticket_flight_without_boarding_pass.ticket_no AND flight_id = ticket_flight_without_boarding_pass.flight_id
            LIMIT 1;
    END LOOP;
END;
$$;
SELECT * from results;
