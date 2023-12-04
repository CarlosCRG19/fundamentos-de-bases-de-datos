#include "book.h"
#include "string.h"

/* Function to create a new Book */
Book *Book_new(int key, long int offset, size_t size, const char *isbn, const char *title, const char *publisher) {
    Book *new_book = (Book *)malloc(sizeof(Book));

    if (new_book != NULL) {
        new_book->index.key = key;
        new_book->index.offset = offset;
        new_book->index.size = size;
        new_book->bookID = key;

        strncpy(new_book->isbn, isbn, sizeof(new_book->isbn) - 1);
        new_book->isbn[sizeof(new_book->isbn) - 1] = '\0'; // Ensure null-termination

        new_book->title = strdup(title); // Copy the title to avoid potential issues
        new_book->publisher = strdup(publisher); // Copy the publisher to avoid potential issues

        // Check for memory allocation errors
        if (new_book->title == NULL || new_book->publisher == NULL) {
            // Roll back and free memory if an error occurs
            free(new_book->title);
            free(new_book->publisher);
            free(new_book);
            return NULL;
        }
    }

    return new_book;
}

