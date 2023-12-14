#include "deleted_book.h"

#include <string.h>

/**
 * Creates a new DeletedBook object with the given parameters.
 *
 * @param offset The offset in the file where the deleted book record is stored.
 * @param size The size of the deleted book record.
 * 
 * @return A pointer to the newly created DeletedBook object.
 */
DeletedBook* DeletedBook_new(long int offset, size_t size) {
    DeletedBook* deleted_book = (DeletedBook*)malloc(sizeof(DeletedBook));

    deleted_book->offset = offset;
    deleted_book->size = size;

    return deleted_book;
}

/**
 * Creates a new DeletedBookArray with the given initial size.
 *
 * @param initial_size The initial size of the deleted book array.
 * 
 * @return A pointer to the newly created DeletedBookArray.
 */
DeletedBookArray* DeletedBookArray_new(size_t initial_size) {
    DeletedBookArray *book_array = (DeletedBookArray *)malloc(sizeof(DeletedBookArray));

    book_array->books = malloc(initial_size * sizeof(DeletedBook));
    book_array->used = 0;
    book_array->size = initial_size;

    return book_array;
}

/**
 * Inserts a DeletedBook at the specified position in the DeletedBookArray.
 *
 * @param array The DeletedBookArray to insert into.
 * @param index The DeletedBook to insert.
 * @param position The position at which to insert the DeletedBook.
 */
void insert_deleted_at(DeletedBookArray *array, DeletedBook *index, int position) {
    if (position < 0 || position > array->used) {
        return;
    }

    if (array->used == array->size) {
        array->size *= 2;
        array->books = realloc(array->books, array->size * sizeof(DeletedBook));
    }

    // Make space for the new element by shifting elements to the right
    memmove(&array->books[position + 1], &array->books[position], (array->used - position) * sizeof(DeletedBook));

    array->books[position] = *index;
    array->used++;
}

/**
 * Frees the memory associated with a DeletedBookArray.
 *
 * @param array The DeletedBookArray to free.
 */
void free_deleted_array(DeletedBookArray* array) {
    free(array->books);
    array->books = NULL;
    array->used = array->size = 0;
    free(array);
}
