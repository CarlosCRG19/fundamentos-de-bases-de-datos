#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int main(void) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char book_ref[10];  // Assuming book_ref is a string with a maximum length of 10 characters

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    printf("Enter book_ref: ");
    fflush(stdout);

    while (fgets(book_ref, sizeof(book_ref), stdin) != NULL) {
        // Remove newline character from the input
        book_ref[strcspn(book_ref, "\n")] = 0;

        if (strlen(book_ref) == 0) {
            printf("Invalid input: book_ref cannot be empty.\n");
        } else {
            char query[512];
            sprintf(query, "SELECT 1 FROM bookings WHERE book_ref = '%s';", book_ref);
            printf("%s\n", query);

            ret = SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);

            if (SQL_SUCCEEDED(ret)) {
                SQLLEN rowCount;
                SQLRowCount(stmt, &rowCount);

                if (rowCount > 0) {
                    printf("book_ref exists.\n");
                } else {
                    printf("Error: book_ref does not exist.\n");
                }
            } else {
                printf("Error executing the query.\n");
            }

            SQLCloseCursor(stmt);
        }

        printf("Enter book_ref: ");
        fflush(stdout);
    }

    printf("\n");

    /* Free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
