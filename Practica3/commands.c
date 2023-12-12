#include "commands.h"

/**
 * Open or create a database file with the given filename and mode.
 * @param filename: Name of the database file
 * @param mode: File access mode ("r" for read, "w" for write, "a" for append, "b" for binary)
 * @return: FILE pointer to the opened or created file
 */
FILE *open_db_file(const char *filename, const char *mode) {
    char filename_with_extension[128];
    snprintf(filename_with_extension, sizeof(filename_with_extension), "%s.db", filename);

    return fopen(filename_with_extension, mode);
}

/**
 * Write book information to a database file.
 * @param filename: Name of the database file
 * @param book: Pointer to the book struct containing book information
 */
void write_book_to_file(const char *filename, Book *book) {
    /* Calculate the size needed to store book information */
    size_t size = sizeof(book->bookID) + sizeof(book->isbn) + strlen(book->title) + strlen(book->publisher) + 1;
    /* Open the database file in append binary mode */
    FILE *file = open_db_file(filename, "ab");

    if (file != NULL) {
        /* Write the size of the book to the file */
        fwrite(&size, sizeof(size_t), 1, file);

        /* Write book information to the file */
        fwrite(&book->bookID, sizeof(int), 1, file);
        fwrite(book->isbn, sizeof(char), sizeof(book->isbn), file);
        fwrite(book->title, sizeof(char), strlen(book->title), file);
        fwrite("|", sizeof(char), 1, file); /* Single '|' separator */
        fwrite(book->publisher, sizeof(char), strlen(book->publisher), file);

        fclose(file);
    } else {
        perror("Error opening file for writing");
    }

    printf("Record with BookID=%d has been added to the database\n", book->bookID);
}

/**
 * Add a book to the database using the information from the 'add' command.
 * @param add_command: 'add' command string containing book information
 * @param output_filename: Name of the database file
 */
void add_book(const char *add_command, Database *db) {
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
    add_book2(db, &new_book);
    /*write_book_to_file(output_filename, &new_book);*/
}

void addBook(Database *db, Book *new_book) {
    // Create a new Book
    if (new_book == NULL) {
        printf("Failed to add book to the database. Memory allocation error.\n");
        return;
    }

    // Resize the books array
    db->records = (Book **)realloc(db->records, (db->size + 1) * sizeof(Book *));
    
    if (db->records == NULL) {
        printf("Failed to add book to the database. Memory allocation error.\n");
        // Roll back and free memory if an error occurs
        /*FREE BOOK(new_book); */
        return;
    }

    // Add the new book to the array
    db->records[db->size] = new_book;
    db->size++;
}

/**
 * Print the offset and the first value of each record in the database file.
 * @param filename: Name of the database file
 */
void print_index(const char *filename) {
    FILE *file = open_db_file(filename, "rb");

    if (file != NULL) {
        int book_number = 0;

        while (!feof(file)) {
            size_t size;
            int book_id, book_offset = ftell(file);

            /* Read the size of the book entry */
            if (fread(&size, sizeof(size_t), 1, file) != 1 || feof(file)) {
                /* Unable to read size, probably reached the end of the file */
                break;
            }

            printf("Entry #%d\n", book_number);

            /* Allocate memory to store the entire book entry */
            char *buffer = (char *)malloc(size);
            if (buffer == NULL) {
                fprintf(stderr, "Memory allocation error.\n");
                exit(EXIT_FAILURE);
            }

            /* Read the entire book entry into the buffer */
            fread(buffer, 1, size, file);

            /* Copy the book key from the buffer */
            memcpy(&book_id, buffer, sizeof(int));

            /* Print the offset and the first value */
            printf("    key: #%d\n", book_id);
            printf("    offset: #%d\n", book_offset);
            printf("    size: #%zu\n", size);

            book_number++;

            /* Free the allocated buffer */
            free(buffer);
        }

        fclose(file);
    } else {
        perror("Error opening file for reading");
   }
}

