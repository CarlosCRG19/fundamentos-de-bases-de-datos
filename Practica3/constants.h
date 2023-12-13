#ifndef CONSTANTS_H
#define CONSTANTS_H

#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2
#define UNKNOWN_STRATEGY 3

enum ReturnStatus {
    OK,
    ERROR,
    MEMORY_ERROR,
    BOOK_ALREADY_EXISTS,
    BOOK_DOESNT_EXISTS,
};

#endif /* CONSTANTS_H */
