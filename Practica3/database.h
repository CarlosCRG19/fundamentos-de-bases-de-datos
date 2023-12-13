#ifndef DATABASE_H
#define DATABASE_H

#include "book.h"
#include "book_index.h"
#include "deleted_book.h"
#include "constants.h"

typedef struct {
    char* data_file;
    char* index_file;
    char* deleted_file;
    int ordering_strategy;
    BookIndexArray* index_array;
    DeletedBookArray* deleted_array;
} Database;

typedef struct {
    int position;
    BookIndex* book_index;
} BookIndexPosition;

Database* Database_new(int ordering_strategy, char *filename);

enum ReturnStatus add_book(Database* db, Book* new_book);

BookIndexPosition find_book(Database* db, int bookID);

Book* get_book(Database* db, BookIndex* book_index);

BookIndexArray* load_index(const char* filename);

enum ReturnStatus save_index(Database* db);

enum ReturnStatus delete_book(Database* db, int bookID);

enum ReturnStatus save_deleted(Database* db);

#endif /* DATABASE_H */
