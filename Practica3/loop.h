/*
* Created by CarlosCRG19 on 15/11/23.
*/


#ifndef LOOP_H
#define LOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"

/**
 * Main loop for user interaction. Reads user commands and processes them until the 'exit' command is entered.
 *
 * @param database Pointer to the Database object.
 *
 * @return None
 */
void loop(Database* database);

/**
 * Process the user command and perform corresponding actions in the database.
 *
 * @param command User-entered command string.
 * @param database Pointer to the Database object.
 *
 * @return None
 */
void process_command(const char *command, Database* database);

#endif /* LOOP_H */
