#include "database.h"
#include "constants.h"
#include "loop.h"

/**
 * Convert a string representation of a search strategy to its corresponding enum value.
 *
 * @param strategy_str The string representation of the search strategy.
 *
 * @return The enum value corresponding to the search strategy. If the strategy is unknown, returns UNKNOWN_STRATEGY.
 */
int to_ordering_stratregy(const char *strategy_str) {
    if (strcmp(strategy_str, "best_fit") == 0) {
        return BESTFIT;
    } else if (strcmp(strategy_str, "worst_fit") == 0) {
        return WORSTFIT;
    } else if (strcmp(strategy_str, "first_fit") == 0) {
        return FIRSTFIT;
    } else {
        return UNKNOWN_STRATEGY;
    }
}

/**
 * Main function that initializes a database and enters a loop for user interaction.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 *
 * @return Exit status. Returns 1 on success, 0 on incorrect arguments or unknown strategy.
 */
int main(int argc, char *argv[]) {
    /** Check if the correct number of arguments is provided **/
    if (argc != 3) {
        printf("Missing argument");
        return 0; /** Return error code if incorrect arguments **/
    }

    /** Extract the arguments **/
    char *ordering_strategy_str = argv[1];
    char *filename = argv[2];

    int ordering_strategy = to_ordering_stratregy(ordering_strategy_str);

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

    /** Free database **/
    free_database(database);

    return 1; /** Return success code **/
}

