#ifndef COMMANDS_H
#define COMMANDS_H

#include "database.h"

/**
 * Add a book to the database using the information from the 'add' command.
 * @param db: Database object 
 * @param add_command: 'add' command string containing book information
 */
void add(Database *db, const char *add_command);

/**
 * Print the offset and the first value of each record in the database file.
 * @param filename: Name of the database file
 */
void print_index(const char *filename);

void print_book(Book *book);

void print_records();

#endif /* COMMANDS_H */
