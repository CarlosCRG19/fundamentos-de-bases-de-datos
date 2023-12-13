#include "database.h"
#include "book_index.h"
#include "deleted_book.h"
#include "enums.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Database* Database_new(enum OrderingStrategy ordering_strategy, char *filename) {
    Database* db = (Database*)malloc(sizeof(Database));

    db->data_file = (char*)malloc(strlen(filename) + 4);  /* ".db" plus null terminator */
    db->index_file = (char*)malloc(strlen(filename) + 5); /* ".ind" plus null terminator */

    strcpy(db->data_file, filename);
    strcpy(db->index_file, filename);

    strcat(db->data_file, ".db");
    strcat(db->index_file, ".ind");

    db->ordering_strategy = ordering_strategy;
    db->index_array = load_index(db->index_file);
    db->deleted_array = DeletedBookArray_new(10);

    return db;
}

/**
 * Write book information to a database file.
 * @param filename: Name of the database file
 * @param book: Pointer to the book struct containing book information
 */
BookIndex* write_book_to_disk(Database* db, Book* book) {
    /* Calculate the size needed to store book information */
    size_t size = sizeof(book->bookID) + sizeof(book->isbn) + strlen(book->title) + strlen(book->publisher) + 1;
    /* Open the database file in append binary mode */
    FILE* file = fopen(db->data_file, "ab");

    if (file != NULL) {
        long int offset = ftell(file);

        /* Write the size of the book to the file */
        fwrite(&size, sizeof(size_t), 1, file);

        /* Write book information to the file */
        fwrite(&book->bookID, sizeof(int), 1, file);
        fwrite(book->isbn, sizeof(char), sizeof(book->isbn), file);
        fwrite(book->title, sizeof(char), strlen(book->title), file);
        fwrite("|", sizeof(char), 1, file); /* Single '|' separator */
        fwrite(book->publisher, sizeof(char), strlen(book->publisher), file);

        fclose(file);

        return BookIndex_new(book->bookID, offset, size);
    } else {
        return NULL;
    }
}

enum ReturnStatus add_book(Database* db, Book* new_book) {
    if (new_book == NULL) {
        printf("Failed to add book to the database. Memory allocation error.\n");
        return MEMORY_ERROR;
    }

    BookIndexPosition bp = find_book(db, new_book->bookID);
    if (bp.book_index != NULL) {
        return BOOK_ALREADY_EXISTS;
    }

    BookIndex* new_index = write_book_to_disk(db, new_book);
    if (new_index == NULL) {
        printf("Error writing book to file.\n");
        return ERROR;
    }

    insert_index_at(db->index_array, new_index, bp.position);

    return OK;
}

BookIndexPosition find_book(Database *db, int bookID) {
    BookIndexPosition result = { -1, NULL };

    size_t left = 0;
    size_t right = db->index_array->used;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (db->index_array->indices[mid].bookID == bookID) {
            // Book found
            result.position = mid;
            result.book_index = &db->index_array->indices[mid];
            return result;
        } else if (db->index_array->indices[mid].bookID < bookID) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // Book not found
    result.position = left;
    result.book_index = NULL;
    return result;
}

Book* get_book(Database* db, BookIndex* book_index) {
    FILE* file = fopen(db->data_file, "rb");

    if (file != NULL) {
        // Seek to the offset of the book in the file
        if (fseek(file, book_index->offset, SEEK_SET) == 0) {
            // Read the size of the book record
            size_t size;
            fread(&size, sizeof(size_t), 1, file);

            // Allocate memory for the book record
            char* record_buffer = (char *)malloc(size);

            if (record_buffer != NULL) {
                // Read the book record into the buffer
                fread(record_buffer, 1, size, file);

                // Create a Book struct and copy data from the buffer
                Book* book = (Book*)malloc(sizeof(Book));

                if (book != NULL) {
                    memcpy(&book->bookID, record_buffer, sizeof(int));
                    // Free the buffer
                    memcpy(book->isbn, record_buffer + sizeof(int), sizeof(book->isbn));
                    // Use strtok to extract title and publisher
                    char *token = strtok(record_buffer + sizeof(int) + sizeof(book->isbn), "|");
                    book->title = strdup(token);

                    token = strtok(NULL, "|");
                    book->publisher = strdup(token);

                    // Free the buffer
                    free(record_buffer);
                    fclose(file);

                    return book;
                } else {
                    printf("Memory allocation error for book.\n");
                }
            } else {
                printf("Memory allocation error for record_buffer.\n");
            }
        } else {
            printf("Error seeking to book offset.\n");
        }

        fclose(file);
    } else {
        printf("Error opening file for reading.\n");
    }

    // If there's an error, return NULL
    return NULL;
}

BookIndexArray *load_index(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return BookIndexArray_new(10);  // Return an empty BookIndexArray
    }

    BookIndexArray *index_array = BookIndexArray_new(10); 

    while (!feof(file)) {
        BookIndex current_index;

        if (fread(&current_index.bookID, sizeof(int), 1, file) != 1) {
            break;  // Break on end of file
        }

        if (fread(&current_index.offset, sizeof(long int), 1, file) != 1) {
            break;  // Break on end of file
        }

        if (fread(&current_index.size, sizeof(size_t), 1, file) != 1) {
            break;  // Break on end of file
        }

        insert_index_at_end(index_array, &current_index);
    }

    fclose(file);

    return index_array;
}

enum ReturnStatus save_index(Database* db) {
    BookIndexArray* index_array = db->index_array;
    FILE* file = fopen(db->index_file, "wb");

    for (size_t i = 0; i < index_array->used; ++i) {
        BookIndex* current_index = &index_array->indices[i];

        fwrite(&current_index->bookID, sizeof(int), 1, file);
        fwrite(&current_index->offset, sizeof(long int), 1, file);
        fwrite(&current_index->size, sizeof(size_t), 1, file);
    }

    fclose(file);
    return OK;
}

int find_deleted_position(Database *db, size_t size) {
    size_t left = 0;
    size_t right = db->deleted_array->used;
    printf("array used: %zu\n", right);
    size_t mid = left + (right - left) / 2;

    while (left < right) {
        if (db->deleted_array->books[mid].size == size) {
            return mid;
        } else if (db->deleted_array->books[mid].size < size) {
            left = mid + 1;
        } else {
            printf("entre aqui 3\n");
            right = mid;
        }
    }

    return left;
}

enum ReturnStatus delete_book(Database* db, int bookID) {
    if (bookID < 0) {
        printf("Invalid bookID given %d.\n", bookID);
        return ERROR;
    }

    BookIndexPosition bp = find_book(db, bookID);
    if (bp.book_index == NULL) {
        return BOOK_DOESNT_EXISTS;
    }

    DeletedBook* deleted_book = DeletedBook_new(bp.book_index->offset, bp.book_index->size);

    int i;
    for (i = 0; i < db->deleted_array->used; ++i) {
        if (db->deleted_array->books[i].size < bp.book_index->size) {
            break;
        }
    }

    insert_deleted_at(db->deleted_array, deleted_book, i);
    delete_index_at(db->index_array, bp.position);

    return OK;
}

