#include "book.h"
#include "string.h"

/**
 * Creates a new Book object with the given parameters.
 *
 * @param bookID The unique identifier for the book.
 * @param isbn The International Standard Book Number of the book.
 * @param title The title of the book.
 * @param publisher The publisher of the book.
 * 
 * @return A pointer to the newly created Book object, or NULL if memory allocation fails.
 */
Book* Book_new(const int bookID, const char* isbn, const char* title, const char* publisher) {
    Book *new_book = (Book*)malloc(sizeof(Book));

    if (new_book != NULL) {
        new_book->bookID = bookID;

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
