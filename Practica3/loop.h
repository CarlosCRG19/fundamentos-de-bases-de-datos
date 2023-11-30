/*
* Created by CarlosCRG19 on 15/11/23.
*/


#ifndef LOOP_H
#define LOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Main loop for user interaction.
 * @param ordering_strategy: Selected book ordering strategy
 * @param filename: Name of the file storing book data
 */
void loop(const char *ordering_strategy, const char *filename);

/**
 * Process the user command.
 * @param command: User-entered command string
 * @param ordering_strategy: Selected book ordering strategy
 * @param filename: Name of the file storing book data
 */
void process_command(const char *command, const char *ordering_strategy, const char *filename);

#endif /* LOOP_H */
