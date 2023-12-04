#ifndef BOOK_H
#define BOOK_H

/* Struct to store book data */
typedef struct Book {
    int bookID;
    char isbn[16];
    char *title;
    char *publisher;
} Book;

#endif /* BOOK_H */
