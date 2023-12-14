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

/**
 * Creates a new DeletedBook object with the given parameters.
 *
 * @param offset The offset in the file where the deleted book record is stored.
 * @param size The size of the deleted book record.
 * 
 * @return A pointer to the newly created DeletedBook object.
 */
DeletedBook* DeletedBook_new(long int offset, size_t size);

/**
 * Creates a new DeletedBookArray with the given initial size.
 *
 * @param initial_size The initial size of the deleted book array.
 * 
 * @return A pointer to the newly created DeletedBookArray.
 */
DeletedBookArray* DeletedBookArray_new(size_t initial_size);

/**
 * Inserts a DeletedBook at the specified position in the DeletedBookArray.
 *
 * @param array The DeletedBookArray to insert into.
 * @param index The DeletedBook to insert.
 * @param position The position at which to insert the DeletedBook.
 */
void insert_deleted_at(DeletedBookArray* array, DeletedBook* index, int position);

/**
 * Frees the memory associated with a DeletedBookArray.
 *
 * @param array The DeletedBookArray to free.
 */
void free_deleted_array(DeletedBookArray* array);

#endif /* DELETED_BOOK_H */
