#include "loop.h"

void process_command(const char *command, const char *ordering_strategy, const char *filename) {
    if (strncmp(command, "add", 3) == 0) {
        /** TODO: Implement the logic for the "add" command **/
        printf("Adding a book to %s with ordering strategy %s.\n", filename, ordering_strategy);
    } else if (strcmp(command, "exit") == 0) {
        /** Inform the user that the program will exit **/
        printf("Exiting the program.\n");
    } else {
        /** Handle unrecognized commands **/
        printf("Unrecognized command: %s\n", command);
    }
}

void loop(const char *ordering_strategy, const char *filename) {
    /** Buffer to store user input **/
    char command_buffer[100];

    while (1) {
        /** Read user input **/
        if (fgets(command_buffer, sizeof(command_buffer), stdin) == NULL) {
            perror("Error reading input");
            return;
        }

        /** Remove newline character from input **/
        command_buffer[strcspn(command_buffer, "\n")] = '\0';

        /** Process the entered command **/
        process_command(command_buffer, ordering_strategy, filename);

        /** Check if the user entered the "exit" command **/
        if (strcmp(command_buffer, "exit") == 0) {
            break; /** Exit the loop **/
        }
    }
}
