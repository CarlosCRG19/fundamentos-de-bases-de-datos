#ifndef BOOK_H
#define BOOK_H

#include "stdlib.h"

/* Struct to store book data */
typedef struct Book {
    struct {
        int key; /* book isbn */
        long int offset ; /* book is stored in disk at this position */
        size_t size; /* book record size */
    } index;
    int bookID;
    char isbn[16];
    char *title;
    char *publisher;
} Book;

#endif /* BOOK_H */
