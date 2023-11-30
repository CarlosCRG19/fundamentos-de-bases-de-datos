#include "loop.h"

/**
 * Main function that accepts two command-line arguments:
 * @param argc: Number of command-line arguments
 * @param argv: Array of command-line argument strings
 * @return: Exit status
 **/
int main(int argc, char *argv[]) {
    /** Check if the correct number of arguments is provided **/
    if (argc != 3) {
        printf("Missing argument");
        return 0; /** Return error code if incorrect arguments **/
    }

    /** Extract the arguments **/
    const char *ordering_strategy = argv[1];
    const char *filename = argv[2];

    const char *valid_strategies[] = {"best_fit", "worst_fit", "first_fit"};
    int is_valid_strategy = 0;

    for (int i = 0; i < (int)(sizeof(valid_strategies) / sizeof(valid_strategies[0])); ++i) {
        if (strcmp(ordering_strategy, valid_strategies[i]) == 0) {
            is_valid_strategy = 1;
            break;
        }
    }

    if (!is_valid_strategy) {
        printf("Unknown search strategy %s", ordering_strategy);
        return 0;  /* Return a non-zero value to indicate an error */
    }

    printf("Type command and argument/s.\n");
    printf("exit\n");

    /** Cycled function that controls user interaction **/
    loop(ordering_strategy, filename);

    return 1; /** Return success code **/
}

