#ifndef AUX_H
#define AUX_H

#include <sql.h>
#include <sqlext.h>

/* CONNECTION PARAMETERS, ADAPT TO YOUR SCENARIO */
#define CONNECTION_PARS "DRIVER=PostgreSQL ANSI;DATABASE=flight;SERVER=localhost;PORT=5432;UID=alumnodb;PWD=alumnodb;"


/* REPORT OF THE MOST RECENT ERROR USING HANDLE handle */
void odbc_extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type);

/* STANDARD CONNECTION PROCEDURE */
int odbc_connect(SQLHENV* env, SQLHDBC* dbc);

/* STANDARD DISCONNECTION PROCEDURE */
int odbc_disconnect(SQLHENV env, SQLHDBC dbc);

#endif
