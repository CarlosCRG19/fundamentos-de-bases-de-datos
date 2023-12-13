#include "book_index.h"

#include <string.h>

BookIndex* BookIndex_new(int bookID, long int offset, size_t size) {
    BookIndex* book_index = (BookIndex*)malloc(sizeof(BookIndex));

    book_index->bookID = bookID;
    book_index->offset = offset;
    book_index->size = size;

    return book_index;
}

BookIndexArray* BookIndexArray_new(size_t initial_size) {
    BookIndexArray *index_array = (BookIndexArray *)malloc(sizeof(BookIndexArray));

    index_array->indices = malloc(initial_size * sizeof(BookIndex));
    index_array->used = 0;
    index_array->size = initial_size;

    return index_array;
}

void insert_index_at_end(BookIndexArray* array, BookIndex* index) {
    if (array->used == array->size) {
        array->size *= 2;
        array->indices = realloc(array->indices, array->size * sizeof(BookIndex));
    }

    array->indices[array->used++] = *index;
}

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

void free_index_array(BookIndexArray* array) {
    free(array->indices);
    array->indices = NULL;
    array->used = array->size = 0;
}
