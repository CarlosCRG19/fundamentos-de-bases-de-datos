/*
* Created by CarlosCRG19 on 15/11/23.
*/


#ifndef LOOP_H
#define LOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loop(const char *ordering_strategy, const char *filename);

/**
 * Function to process different commands
 * @param command: Entered command
 * @param ordering_strategy: Strategy for ordering
 * @param filename: Name of the file
 **/
void process_command(const char *command, const char *ordering_strategy, const char *filename);

#endif /* LOOP_H */
