#ifndef ENUMS_H
#define ENUMS_H

enum OrderingStrategy {
    BEST_FIT,
    WORST_FIT,
    FIRST_FIT,
    UNKNOWN_STRATEGY
};

enum ReturnStatus {
    OK,
    ERROR,
    MEMORY_ERROR,
    BOOK_ALREADY_EXISTS,
    BOOK_DOESNT_EXISTS,
};

#endif /* ENUMS_H */
