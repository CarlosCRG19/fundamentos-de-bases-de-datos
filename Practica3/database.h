#ifndef DATABASE_H
#define DATABASE_H

#include "book.h"
#include "book_index.h"
#include "enums.h"

typedef struct {
    char *filename;
    enum OrderingStrategy ordering_strategy;
    BookIndexArray *index_array;
} Database;

typedef struct {
    int position;
    BookIndex *book_index;
} BookIndexPosition;

Database* Database_new(enum OrderingStrategy ordering_strategy, char *filename);

int add_book(Database *db, Book *new_book);

BookIndexPosition find_book(Database *db, int bookID);

Book* get_book(Database *db, BookIndex *book_index);


#endif /* DATABASE_H */
