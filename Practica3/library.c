#include "database.h"
#include "enums.h"
#include "loop.h"

enum OrderingStrategy to_ordering_stratregy(const char *strategy_str) {
    if (strcmp(strategy_str, "best_fit") == 0) {
        return BEST_FIT;
    } else if (strcmp(strategy_str, "worst_fit") == 0) {
        return WORST_FIT;
    } else if (strcmp(strategy_str, "first_fit") == 0) {
        return FIRST_FIT;
    } else {
        return UNKNOWN_STRATEGY;
    }
}

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
    char *ordering_strategy_str = argv[1];
    char *filename = argv[2];

    enum OrderingStrategy ordering_strategy = to_ordering_stratregy(ordering_strategy_str);

    if (ordering_strategy == UNKNOWN_STRATEGY) {
        printf("Unknown search strategy %s", ordering_strategy_str);
        return 0;  /* Return a non-zero value to indicate an error */
    }

    printf("Type command and argument/s.\n");
    printf("exit\n");

    /** Create new database **/
    Database* database = Database_new(ordering_strategy, filename);

    /** Cycled function that controls user interaction **/
    loop(database);

    /** TODO: Free database **/

    return 1; /** Return success code **/
}

