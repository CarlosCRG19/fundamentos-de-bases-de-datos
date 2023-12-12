#ifndef BOOK_H
#define BOOK_H

#include <stdlib.h>

/* Struct to store book data */
typedef struct {
    int bookID;
    char isbn[16];
    char *title;
    char *publisher;
} Book;

Book* Book_new(const int bookID, const char* isbn, const char* title, const char* publisher);

#endif /* BOOK_H */
