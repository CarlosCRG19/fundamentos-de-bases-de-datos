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
        fprintf(stderr, "Usage: %s <ordering_strategy> <filename>\n", argv[0]);
        return 1; /** Return error code if incorrect arguments **/
    }

    /** Extract the arguments **/
    const char *ordering_strategy = argv[1];
    const char *filename = argv[2];

    printf("Type command and argument/s.\n");
    printf("exit\n");

    loop(ordering_strategy, filename);

    return 0; /** Return success code **/
}

