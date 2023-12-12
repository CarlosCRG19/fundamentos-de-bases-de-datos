#include "loop.h"
#include "commands.h"

/**
 * Process the user command.
 * @param command: User-entered command string
 * @param ordering_strategy: Selected book ordering strategy
 * @param filename: Name of the file storing book data
 */
void process_command(const char *command, Database* database) {
    if (strncmp(command, "add", 3) == 0) {
        add(database, command);
        printf("exit\n");  /* Notify the user of command completion */
    } 
    else if (strncmp(command, "find", 4) == 0) {
        find(database, command);
        printf("exit\n");  /* Notify the user of command completion */
    } 
    else if (strcmp(command, "printInd") == 0) {
        /* Print information about the stored books */
        printInd(database);
        printf("exit\n");  /* Notify the user of command completion */
    } 
    else if (strcmp(command, "printRec") == 0) {
        /* Print information about the stored books */
        printRec(database);
        printf("exit\n");  /* Notify the user of command completion */
    } 
    else if (strcmp(command, "exit") == 0) {
        /* Inform the user that the program is done and will exit */
        save_index(database);
        printf("all done\n");
    } 
    else {
        printf("Unrecognized command: %s\n", command);
    }
}

/**
 * Main loop for user interaction.
 * @param ordering_strategy: Selected book ordering strategy
 * @param filename: Name of the file storing book data
 */
void loop(Database *database) {
    char command_buffer[100];

    /* Continue looping until the user enters the 'exit' command */
    while (1) {
        /* Read user input */
        if (fgets(command_buffer, sizeof(command_buffer), stdin) == NULL) {
            printf("Error reading input");
            return;
        }

        /* Remove newline character from input */
        command_buffer[strcspn(command_buffer, "\n")] = '\0';

        process_command(command_buffer, database);

        /* Check if the user entered the 'exit' command to break out of the loop */
        if (strcmp(command_buffer, "exit") == 0) {
            printf("reached here\n");
            break;
        }
    }
}

