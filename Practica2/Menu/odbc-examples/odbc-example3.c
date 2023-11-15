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

    char result[1024], query[512], temp[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        
    SQLCHAR aircraft_code[8], scheduled_departure[64], scheduled_arrival[64];

    printf("x = ");
    fflush(stdout);
    while (fgets(x, sizeof(x), stdin) != NULL) {
        char query[3500];

        sprintf(query, "SELECT aircraft_code, scheduled_departure, scheduled_arrival \
                        FROM flights \
                        WHERE flight_id = '27342' OR flight_id = -1 \
                        ORDER BY scheduled_departure ASC;");

        SQLExecDirect(stmt, (SQLCHAR *)query, SQL_NTS);

        SQLBindCol(stmt, 1, SQL_C_CHAR, aircraft_code, sizeof(aircraft_code), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, scheduled_departure, sizeof(scheduled_departure), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, scheduled_arrival, sizeof(scheduled_arrival), NULL);


        printf("%s", query); 

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        SQLBindCol(stmt, 1, SQL_C_CHAR, y, sizeof(y), NULL);
        result[0] = '\0';  
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            sprintf(temp, "FID: %s, AC: %s, SD: %s, SA: %s\n", "dummy", aircraft_code, scheduled_departure, scheduled_arrival);
            strcat(result, temp);
        }

        if (result[0] != '\0') {
            result[strlen(result) - 1] = '\0';
            printf("%s", result);
        } else {
            printf("falloooo\n");
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
