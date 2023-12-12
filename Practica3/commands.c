#include "commands.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Add a book to the database using the information from the 'add' command.
 * @param add_command: 'add' command string containing book information
 * @param output_filename: Name of the database file
 */
void add(Database *db, const char *add_command) {
    Book new_book;
    char book_data[128];

    /* Skip "add " */
    add_command += 4;
    strcpy(book_data, add_command);

    /* Extract book content */

    char *token = strtok(book_data, "|");
    new_book.bookID = atoi(token);

    token = strtok(NULL, "|");
    strncpy(new_book.isbn, token, sizeof(new_book.isbn));

    token = strtok(NULL, "|");
    new_book.title = strdup(token);

    token = strtok(NULL, "|");
    new_book.publisher = strdup(token);

    /* Write the new book to the database file */
    if (add_book(db, &new_book)) {
        // handle success
        printf("Record with BookID=%d has been added to the database\n", new_book.bookID);
    } else {
        // handle error
    }
    /*write_book_to_file(output_filename, &new_book);*/
}

void print_index(const char *filename) {
}
