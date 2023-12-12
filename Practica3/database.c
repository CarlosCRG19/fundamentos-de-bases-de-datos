#include "database.h"
#include "book_index.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Database* Database_new(enum OrderingStrategy ordering_strategy, char *filename) {
    Database* db = (Database*)malloc(sizeof(Database));
    db->filename = filename;
    db->ordering_strategy = ordering_strategy;
    db->index_array = BookIndexArray_new(50);

    return db;
}

/**
 * Open or create a database file with the given filename and mode.
 * @param filename: Name of the database file
 * @param mode: File access mode ("r" for read, "w" for write, "a" for append, "b" for binary)
 * @return: FILE pointer to the opened or created file
 */
FILE *open_db_file(Database *db, const char *mode) {
    char filename_with_extension[128];
    snprintf(filename_with_extension, sizeof(filename_with_extension), "%s.db", db->filename);

    return fopen(filename_with_extension, mode);
}

/**
 * Write book information to a database file.
 * @param filename: Name of the database file
 * @param book: Pointer to the book struct containing book information
 */
long int write_book_to_disk(Database *db, Book *book) {
    /* Calculate the size needed to store book information */
    size_t size = sizeof(book->bookID) + sizeof(book->isbn) + strlen(book->title) + strlen(book->publisher) + 1;
    /* Open the database file in append binary mode */
    FILE *file = open_db_file(db, "ab");

    if (file != NULL) {
        long int write_position = ftell(file);

        /* Write the size of the book to the file */
        fwrite(&size, sizeof(size_t), 1, file);

        /* Write book information to the file */
        fwrite(&book->bookID, sizeof(int), 1, file);
        fwrite(book->isbn, sizeof(char), sizeof(book->isbn), file);
        fwrite(book->title, sizeof(char), strlen(book->title), file);
        fwrite("|", sizeof(char), 1, file); /* Single '|' separator */
        fwrite(book->publisher, sizeof(char), strlen(book->publisher), file);

        fclose(file);

        return write_position;
    } else {
        perror("Error opening file for writing");
        return -1;
    }

    return 0;
}

int add_book(Database *db, Book *new_book) {
    if (new_book == NULL) {
        printf("Failed to add book to the database. Memory allocation error.\n");
        return 0;
    }

    write_book_to_disk(db, new_book);

    return 1; // return success
}
