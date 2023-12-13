#ifndef DELETED_BOOK_H
#define DELETED_BOOK_H

#include <stdlib.h>

/* Structure of deleted book */
typedef struct {
    long int offset ; /* book is stored in disk at this position */
    size_t size; /* book record size */
} DeletedBook;


/* Dynamic deleted book array */
typedef struct {
    DeletedBook *books;
    size_t used;
    size_t size;
} DeletedBookArray;

DeletedBook* DeletedBook_new(long int offset, size_t size);

DeletedBookArray* DeletedBookArray_new(size_t initial_size);

void insert_deleted_at(DeletedBookArray* array, DeletedBook* index, int position);

void free_deleted_array(DeletedBookArray* array);

#endif /* DELETED_BOOK_H */
