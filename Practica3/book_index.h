#ifndef BOOK_INDEX_H
#define BOOK_INDEX_H

#include "stdlib.h"

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

BookIndex *BookIndex_new(int bookID, long int offset, size_t size);

BookIndexArray *BookIndexArray_new(size_t initial_size);

void insert_at(BookIndexArray *array, BookIndex *index, int position);

void insert_at_end(BookIndexArray *array, BookIndex *index);

void free_array(BookIndexArray *array);

#endif /* BOOK_INDEX_H */
