#ifndef COMMANDS_H
#define COMMANDS_H

#include "database.h"

/**
 * Adds a book to the database using the information from the 'add' command.
 *
 * @param db The Database object.
 * @param add_command The 'add' command string containing book information.
 * 
 * @return None
 */
void add(Database* db, const char* add_command);

/**
 * Finds and prints information about a book in the database based on the 'find' command.
 *
 * @param db The Database object.
 * @param find_command The 'find' command string containing bookID information.
 * 
 * @return None
 */
void find(Database* db, const char* find_command);

/**
 * Prints the information stored in the index array of the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printInd(Database* db);

/**
 * Prints the detailed information of all books in the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printRec(Database* db);

/**
 * Prints the information stored in the deleted array of the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printLst(Database* db);

/**
 * Deletes a book from the database based on the 'del' command.
 *
 * @param db The Database object.
 * @param del_command The 'del' command string containing bookID information.
 * 
 * @return None
 */
void del(Database* db, const char* del_command);

#endif /* COMMANDS_H */
