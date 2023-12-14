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

/**
 * Creates a new Book object with the given parameters.
 *
 * @param bookID The unique identifier for the book.
 * @param isbn The International Standard Book Number of the book.
 * @param title The title of the book.
 * @param publisher The publisher of the book.
 * 
 * @return A pointer to the newly created Book object, or NULL if memory allocation fails.
 */
Book* Book_new(const int bookID, const char* isbn, const char* title, const char* publisher);

#endif /* BOOK_H */
