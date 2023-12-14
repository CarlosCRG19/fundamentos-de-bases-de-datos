#include "deleted_book.h"
DeletedBookArray *load_index(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return DeletedBookArray_new(10);  // Return an empty DeletedBookArray
    }

    DeletedBookArray *deleted_array = DeletedBookArray_new(10); 

    while (!feof(file)) {
        DeletedBook deleted_book;

        if (fread(&deleted_book.offset, sizeof(long int), 1, file) != 1) {
            break;  // Break on end of file
        }

        if (fread(&deleted_book.size, sizeof(size_t), 1, file) != 1) {
            break;  // Break on end of file
        }

        insert_deleted_at(deleted_array, deleted_book, deleted_array->used)
    }

    fclose(file);

    return deleted_array;
}
