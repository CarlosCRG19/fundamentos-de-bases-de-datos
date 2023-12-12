#include "book_index.h"

BookIndexArray *BookIndexArray_new(size_t initial_size) {
    BookIndexArray *index_array = (BookIndexArray *)malloc(sizeof(BookIndexArray));

    index_array->indices = malloc(initial_size * sizeof(BookIndex));
    index_array->used = 0;
    index_array->size = initial_size;

    return index_array;
}
