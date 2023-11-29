#include "loop.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura para almacenar información sobre un libro
struct Book {
    int bookID;
    char isbn[16];
    char *title;
    char *publisher;
};

// Función para liberar la memoria asignada a un libro
void free_book(struct Book *book) {
    free(book->isbn);
    free(book->title);
    free(book->publisher);
}

FILE *open_db_file(const char *filename, const char *mode) {
    char filename_with_extension[128];
    snprintf(filename_with_extension, sizeof(filename_with_extension), "%s.db", filename);

    return fopen(filename_with_extension, mode);
}

// Función para escribir libros en un archivo
void write_book_to_file(const char *filename, struct Book *book) {
    size_t size = sizeof(book->bookID) + sizeof(book->isbn) + strlen(book->title) + strlen(book->publisher) + 1;
    FILE *file = open_db_file(filename, "ab");

    if (file != NULL) {
        // Write the size of the book to the file
        fwrite(&size, sizeof(size_t), 1, file);

        // Write the key, isbn, title, separator, and editorial to the file
        fwrite(&book->bookID, sizeof(int), 1, file);
        fwrite(book->isbn, sizeof(char), sizeof(book->isbn), file);
        fwrite(book->title, sizeof(char), strlen(book->title), file);
        fwrite("|", sizeof(char), 1, file); // Single '|' separator
        fwrite(book->publisher, sizeof(char), strlen(book->publisher), file);

        fclose(file);
    } else {
        perror("Error opening file for writing");
    }

    printf("Record with BookID=%d has been added to the database\n", book->bookID);
}

void add_book(const char *add_command, const char* output_filename) {
    struct Book new_book;
    char book_data[128];

    /* Skip "add " */
    add_command += 4;
    strcpy(book_data, add_command);

    /* Extract bookID */
    char *token = strtok(book_data, "|");
    new_book.bookID = atoi(token);

    /* Extract ISBN */
    token = strtok(NULL, "|");
    strncpy(new_book.isbn, token, sizeof(new_book.isbn));

    /* Extract Title */
    token = strtok(NULL, "|");
    new_book.title = strdup(token);

    /* Extract Publisher */
    token = strtok(NULL, "|");
    new_book.publisher = strdup(token);

    write_book_to_file(output_filename, &new_book);
}


// Función para imprimir el offset y el primer valor de cada registro
void print_index(const char *filename) {
    FILE *file = open_db_file(filename, "rb");

    if (file != NULL) {
        int book_number = 0;

        while (!feof(file)) {
            size_t size;
            int book_id, book_offset = ftell(file);

            if (fread(&size, sizeof(size_t), 1, file) != 1 || feof(file)) {
                // No se pudo leer el tamaño, probablemente llegamos al final del archivo
                break;
            }

            printf("Entry #%d\n", book_number);

            /* Create entry to store book data */
            char *buffer = (char *)malloc(size);
            if (buffer == NULL) {
                fprintf(stderr, "Memory allocation error.\n");
                exit(EXIT_FAILURE);
            }

            /* Extract entire book entry */
            fread(buffer, 1, size, file);

            /* Copy book key */
            memcpy(&book_id, buffer, sizeof(int));

            // Imprimir el offset y el primer valor
            printf("    key: #%d\n", book_id);
            printf("    offset: #%d\n", book_offset);

            book_number++;
        }

        fclose(file);
    } else {
        perror("Error opening file for reading");
    }
}

// Función para procesar comandos
void process_command(const char *command, const char *ordering_strategy, const char *filename) {
    if (strncmp(command, "add", 3) == 0) {
        add_book(command, filename);
        printf("exit\n");
    } else if (strcmp(command, "printInd") == 0) {
        // Imprima la información de los libros almacenados
        print_index(filename);
        printf("exit\n");
    } else if (strcmp(command, "exit") == 0) {
        // Informe al usuario que el programa va a salir
        printf("all done\n");
    } else {
        // Maneje comandos no reconocidos
        printf("Unrecognized command: %s\n", command);
    }
}

// Función principal del bucle
void loop(const char *ordering_strategy, const char *filename) {
    /** Buffer to store user input **/
    char command_buffer[100];

    while (1) {
        /** Read user input **/
        if (fgets(command_buffer, sizeof(command_buffer), stdin) == NULL) {
            perror("Error reading input");
            return;
        }

        /** Remove newline character from input **/
        command_buffer[strcspn(command_buffer, "\n")] = '\0';

        /** Process the entered command **/
        process_command(command_buffer, ordering_strategy, filename);

        if (strcmp(command_buffer, "exit") == 0) {
            break; /** Exit the loop **/
        }
    }
}
