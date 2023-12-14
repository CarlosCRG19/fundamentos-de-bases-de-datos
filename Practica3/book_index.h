#ifndef BOOK_INDEX_H
#define BOOK_INDEX_H

#include <stdlib.h>

/* Index structure */
typedef struct {
    int bookID; /* book id */
    long int offset ; /* book is stored in disk at this position */
    size_t size; /* book record size */
} BookIndex;


/* Dynamic index array */
typedef struct {
    BookIndex *indices;
    size_t used;
    size_t size;
} BookIndexArray;

/**
 * Creates a new BookIndex object with the given parameters.
 *
 * @param bookID The unique identifier for the book index.
 * @param offset The offset in the file where the book record is stored.
 * @param size The size of the book record.
 * 
 * @return A pointer to the newly created BookIndex object.
 */
BookIndex* BookIndex_new(int bookID, long int offset, size_t size);

/**
 * Creates a new BookIndexArray with the given initial size.
 *
 * @param initial_size The initial size of the index array.
 * 
 * @return A pointer to the newly created BookIndexArray.
 */
BookIndexArray* BookIndexArray_new(size_t initial_size);

/**
 * Inserts a BookIndex at the end of the BookIndexArray.
 *
 * @param array The BookIndexArray to insert into.
 * @param index The BookIndex to insert.
 */
void insert_index_at(BookIndexArray* array, BookIndex* index, int position);

/**
 * Inserts a BookIndex at the specified position in the BookIndexArray.
 *
 * @param array The BookIndexArray to insert into.
 * @param index The BookIndex to insert.
 * @param position The position at which to insert the BookIndex.
 */
void insert_index_at_end(BookIndexArray* array, BookIndex* index);

/**
 * Deletes the BookIndex at the specified position in the BookIndexArray.
 *
 * @param array The BookIndexArray to delete from.
 * @param position The position of the BookIndex to delete.
 */
void delete_index_at(BookIndexArray *array, int position);

/**
 * Frees the memory associated with a BookIndexArray.
 *
 * @param array The BookIndexArray to free.
 */
void free_index_array(BookIndexArray* array);

#endif /* BOOK_INDEX_H */
