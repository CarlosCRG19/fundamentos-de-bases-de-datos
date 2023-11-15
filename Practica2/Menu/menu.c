/*
 Created by roberto on 30/4/21.
 Template to create a set of windows that will display
 a menu, a couple of forms and some space for output
 and mesages
 (see image bellow)

 This file will divide the screen as follows and
 create a menu

 +-------------------------------+ <-- main_win
 |+-----------------------------+|
 ||          win_menu           ||
 |+-----------------------------+|
 |+--------++-------------------+|
 ||        ||                   ||
 ||        ||                   ||
 ||win_form||   win_out         ||
 ||        ||                   ||
 ||        ||                   ||
 |+--------++-------------------+|
 |+-----------------------------+|
 ||          win_messages        ||
 |+-----------------------------+|
 +-------------------------------+

 */
#include <sqltypes.h>
#include <stdlib.h>
#include <string.h>
#include "lmenu.h" /* menu.h is a ncurses header */
#include "odbc.h"
#include "utils.h"
#include "windows.h"

const char* _CREATE_BOARDING_PASSES_FUNCTION= "CREATE OR REPLACE FUNCTION create_boarding_passes(book_ref_param TEXT) "
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

static void init_struct(_Windows *windows, __attribute__((unused)) _Panels *panels,
                 _Menus *menus, _Forms *forms)
/** Functions that initialices windows, menus, forms, etc. Note that the initializacion is
 * only partial. The rest will be done in _initsrc after initializating the curses framework
 *
 * @param windows, pointers to windows
 * @param panels, pointers to panels
 * @param menus, pointers to menus
 * @param forms, pointers to forms
 */
{
    /* windows and panels do not require initialization */
    /* menu and form initialization */
    int i;
    /* labels for menu places in win_menu */
    char *menu_choices[] = {
            "Search",
            "B. Pass",
            "Quit"
    };

    /* labels for forms placed in win_form */
    char *forms_search_choices[] = {
            "From: ",
            "To:   ",
            "Date: "
    };
    char *forms_bpass_choices[] = {
            "book Id: "
    };

    /* Menu: copy menu labels to menu structure */
    menus->no_items = (int)(sizeof(menu_choices) / sizeof(menu_choices[0]));
    menus->choices = (char **) malloc(sizeof(char *) * menus->no_items);
    if(menus->choices == NULL)
         exit(0);

    for (i = 0; i < menus->no_items; i++) {
        (menus->choices)[i] = (char *) malloc(
                sizeof(char *) * sizeof(menu_choices[i]) + 1);
        assert(menus->choices[i] != NULL);
        strcpy((menus->choices)[i], menu_choices[i]);
    }

    /* default windows (menu) size */
    windows->height_menu_win = 3;
    windows->width_form_win = 20;

    /* Menu: copy form labels to form structure */
    forms->no_items_search = (int)(sizeof(forms_search_choices) /
            sizeof(forms_search_choices[0]));
    forms->choices_search_form = (char **) malloc(
            sizeof(char *) * forms->no_items_search);
    for (i = 0; i < forms->no_items_search; i++) {
        (forms->choices_search_form)[i] =
                (char *) malloc(sizeof(char *) *
                sizeof(forms_search_choices[i]) + 1);
        strcpy((forms->choices_search_form)[i],
               forms_search_choices[i]);
    }

    /* Form bpass */
    forms->no_items_bpass = (int)(sizeof(forms_bpass_choices) / sizeof(forms_bpass_choices[0]));
    forms->choices_bpass_form = (char **) malloc(sizeof(char *) * forms->no_items_bpass);
    for (i = 0; i < forms->no_items_bpass; i++) {
        (forms->choices_bpass_form)[i] = (char *) malloc(sizeof(char *) * sizeof(forms_bpass_choices[i]) + 1);
        strcpy((forms->choices_bpass_form)[i], forms_bpass_choices[i]);
    }

    /* store titles for the different windows */
    windows->form_search_title =
            (char *) malloc(sizeof(char *) * sizeof("Form Search")+1);
    strcpy(windows->form_search_title, "Form Search");

    windows->form_bpass_title =
            (char *) malloc(sizeof(char *) * sizeof("Form Bpass")+1);
    strcpy(windows->form_bpass_title, "Form Bpass");

    windows->menu_title =
            (char *) malloc(sizeof(char *) * sizeof("Menu")+1);
    strcpy(windows->menu_title, "Menu");

    windows->out_title =
            (char *) malloc(sizeof(char *) * sizeof("Out")+1);
    strcpy(windows->out_title, "Out");

    windows->msg_title =
            (char *) malloc(sizeof(char *) * sizeof("Msg")+1);
    strcpy(windows->msg_title, "Msg");
}

static void init_statements(_PreparedStatements *statements, SQLHDBC dbc) {
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &(statements->flight_connections));
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &(statements->flights_details));
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &(statements->booking_check));
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &(statements->create_boardin_passes_function));
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &(statements->created_boarding_passes));

    SQLPrepare(statements->flight_connections, (SQLCHAR *)"SELECT * FROM "
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
                   "ORDER BY scheduled_arrival - scheduled_departure ASC;", SQL_NTS);

    SQLPrepare(statements->flights_details, (SQLCHAR *)"SELECT aircraft_code, scheduled_departure, scheduled_arrival \
                        FROM flights \
                        WHERE flight_id = ? OR flight_id = ? \
                        ORDER BY scheduled_departure ASC;", SQL_NTS);

    SQLPrepare(statements->booking_check, (SQLCHAR *)"SELECT 1 FROM bookings WHERE book_ref = ? LIMIT 1;", SQL_NTS);

    SQLPrepare(statements->created_boarding_passes, (SQLCHAR *)"SELECT * FROM create_boarding_passes(?);", SQL_NTS);
}

static void free_struct(_Windows windows, _Panels panels,
                 _Menus menus, _Forms forms)
     /** free memory related with structures windows,
      * panels, forms and menus
      *
      * Note regarding valgrind
      *  valgrind --leak-check=full --show-reachable=yes
While I'm not 100% familiar with the internals of ncurses, I think the
 main problem is that ncurses keeps a lot of state in global variables.
 This state is allocated with initscr at the beginning, but never
 deallocated afterwards. Similar memory “leaks” occur when using standard IO
 functions; a buffer is allocated for stdin, stdout, and stderr on first use
 and this buffer is only deallocated when you close these files.
 I wouldn't worry about them too much.
 by 0x10923E: _initsrc (windows.c:35)

The leaks you should worry about are the “unreachable” leaks because they indicate
 memory that can no longer be released by any means because you forgot all pointers to it.


      * @param windows
      * @param panels
      * @param menus
      * @param forms
      */
{
    int i;
    /* free windows */
    free(windows.msg_title);
    free(windows.out_title);
    free(windows.form_search_title);
    free(windows.form_bpass_title);
    free(windows.menu_title);

    unpost_form(forms.search_form);
    unpost_form(forms.bpass_form); 
    unpost_menu(menus.menu);
    free_form(forms.search_form);
    free_form(forms.bpass_form);
    free_menu(menus.menu);

    for (i = 0; i < menus.no_items; i++){
        free_item((menus.menuitems)[i]);
        free(menus.choices[i]);
    }
    free(menus.choices);
    free(menus.menuitems);

    /* free forms */
    for (i = 0; i < 2*forms.no_items_search+1; i++){
        free_field(forms.search_form_items[i]);
    }
    for (i = 0; i < forms.no_items_search; i++)
        free(forms.choices_search_form[i]);
    free(forms.choices_search_form);
    free(forms.search_form_items);

    for (i = 0; i < forms.no_items_bpass; i++)
        free(forms.choices_bpass_form[i]);
    for (i = 0; i < 2*forms.no_items_bpass+1; i++)
        free_field(forms.bpass_form_items[i]);
    free(forms.choices_bpass_form);
    free(forms.bpass_form_items);

    for (i = 0; i < windows.rows_out_win; i++)
        free(menus.out_win_choices[i]);
    free(menus.out_win_choices);

    /*free panels */
    del_panel(panels.search_panel);
    del_panel(panels.bpass_panel);
    /* free windows */
    delwin(windows.msg_win);
    delwin(windows.out_win);
    delwin(windows.form_bpass_win);
    delwin(windows.form_search_win);
    delwin(windows.menu_win);
    delwin(windows.main_win);

    endwin();
}

static void free_handles(_PreparedStatements statements) {
    SQLFreeHandle(SQL_HANDLE_STMT, statements.flight_connections);
    SQLFreeHandle(SQL_HANDLE_STMT, statements.flights_details);
    SQLFreeHandle(SQL_HANDLE_STMT, statements.booking_check);
    SQLFreeHandle(SQL_HANDLE_STMT, statements.created_boarding_passes);
}

int main() {
    _Windows windows;
    _Panels panels;
    _Menus menus;
    _Forms forms;
    _PreparedStatements statements;

    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret; /* ODBC API return status */

    /* copy default values to data structures */
    init_struct(&windows, &panels, &menus, &forms);

    /* create windows, menus, panels, etc */
    _initsrc(&windows, &menus, &forms, &panels);

    /* create connection to db */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        write_error(windows.msg_win, "could not connect to database");
    }

    init_statements(&statements, dbc);

    /* process keyboard */
    loop(&windows, &menus, &forms, &panels, &statements);

    /* free memory */
    free_struct(windows, panels, menus, forms);

    /* free statement handles */
    free_handles(statements);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return 0;
}
