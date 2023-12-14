#include "book_index.h"

#include <string.h>

/**
 * Creates a new BookIndex object with the given parameters.
 *
 * @param bookID The unique identifier for the book index.
 * @param offset The offset in the file where the book record is stored.
 * @param size The size of the book record.
 * 
 * @return A pointer to the newly created BookIndex object.
 */
BookIndex* BookIndex_new(int bookID, long int offset, size_t size) {
    BookIndex* book_index = (BookIndex*)malloc(sizeof(BookIndex));

    book_index->bookID = bookID;
    book_index->offset = offset;
    book_index->size = size;

    return book_index;
}

/**
 * Creates a new BookIndexArray with the given initial size.
 *
 * @param initial_size The initial size of the index array.
 * 
 * @return A pointer to the newly created BookIndexArray.
 */
BookIndexArray* BookIndexArray_new(size_t initial_size) {
    BookIndexArray *index_array = (BookIndexArray *)malloc(sizeof(BookIndexArray));

    index_array->indices = malloc(initial_size * sizeof(BookIndex));
    index_array->used = 0;
    index_array->size = initial_size;

    return index_array;
}

/**
 * Inserts a BookIndex at the end of the BookIndexArray.
 *
 * @param array The BookIndexArray to insert into.
 * @param index The BookIndex to insert.
 */
void insert_index_at_end(BookIndexArray* array, BookIndex* index) {
    if (array->used == array->size) {
        array->size *= 2;
        array->indices = realloc(array->indices, array->size * sizeof(BookIndex));
    }

    array->indices[array->used++] = *index;
}

/**
 * Inserts a BookIndex at the specified position in the BookIndexArray.
 *
 * @param array The BookIndexArray to insert into.
 * @param index The BookIndex to insert.
 * @param position The position at which to insert the BookIndex.
 */
void insert_index_at(BookIndexArray *array, BookIndex *index, int position) {
    if (position < 0 || position > array->used) {
        return;
    }

    if (array->used == array->size) {
        array->size *= 2;
        array->indices = realloc(array->indices, array->size * sizeof(BookIndex));
    }

    // Make space for the new element by shifting elements to the right
    memmove(&array->indices[position + 1], &array->indices[position], (array->used - position) * sizeof(BookIndex));

    array->indices[position] = *index;
    array->used++;
}

/**
 * Deletes the BookIndex at the specified position in the BookIndexArray.
 *
 * @param array The BookIndexArray to delete from.
 * @param position The position of the BookIndex to delete.
 */
void delete_index_at(BookIndexArray *array, int position) {
    if (position < 0 || position >= array->used) {
        return; // Invalid position
    }

    // Shift elements to the left to overwrite the element at the specified position
    memmove(&array->indices[position], &array->indices[position + 1], (array->used - position - 1) * sizeof(BookIndex));

    array->used--;

    // If the used count is significantly smaller than the array size, shrink the array
    if (array->used > 0 && array->used < array->size / 2) {
        array->size /= 2;
        array->indices = realloc(array->indices, array->size * sizeof(BookIndex));
    }
}

/**
 * Frees the memory associated with a BookIndexArray.
 *
 * @param array The BookIndexArray to free.
 */
void free_index_array(BookIndexArray* array) {
    free(array->indices);
    array->indices = NULL;
    array->used = array->size = 0;
}

