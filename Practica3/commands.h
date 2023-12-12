#ifndef COMMANDS_H
#define COMMANDS_H

#include "database.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Add a book to the database using the information from the 'add' command.
 * @param add_command: 'add' command string containing book information
 * @param output_filename: Name of the database file
 */
void add_book(const char *add_command, const char* output_filename);
void add_book2(Database *db, Book *new_book);

/**
 * Print the offset and the first value of each record in the database file.
 * @param filename: Name of the database file
 */
void print_index(const char *filename);

#endif /* COMMANDS_H */
