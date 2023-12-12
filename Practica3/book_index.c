#include "book_index.h"

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

void insert_at_end(BookIndexArray* array, BookIndex* index) {
    if (array->used == array->size) {
        array->size *= 2;
        array->indices = realloc(array->indices, array->size * sizeof(BookIndex));
    }

    array->indices[array->used++] = *index;
}

void free_array(BookIndexArray* array) {
    free(array->indices);
    array->indices = NULL;
    array->used = array->size = 0;
}
