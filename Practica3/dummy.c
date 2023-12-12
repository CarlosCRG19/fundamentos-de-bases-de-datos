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

