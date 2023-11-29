#include "loop.h"
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

// Función para escribir libros en un archivo
void write_book_to_file(const char *filename, struct Book *book) {
    size_t size = sizeof(book->bookID) + sizeof(book->isbn) + strlen(book->title) + strlen(book->publisher) + 1;
    FILE *file = fopen(filename, "wb");

    if (file != NULL) {
        // Write the size of the book to the file
        fwrite(&size, sizeof(size_t), 1, file);

        // Write the key, isbn, title, separator, and editorial to the file
        fwrite(&book->isbn, sizeof(int), 1, file);
        fwrite(book->isbn, sizeof(char), sizeof(book->isbn), file);
        fwrite(book->title, sizeof(char), strlen(book->title), file);
        fwrite("|", sizeof(char), 1, file); // Single '|' separator
        fwrite(book->publisher, sizeof(char), strlen(book->publisher), file);

        fclose(file);
    } else {
        perror("Error opening file for writing");
    }

    printf("Record with BookID=%d has been added to the database\n", book->bookID);
    printf("exit\n");
}

// Función para leer libros desde un archivo
void read_books_from_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        bookCount = fread(books, sizeof(struct Book), 100, file);  // Use 100 as the maximum number of elements
        fclose(file);
    } else {
        perror("Error opening file for reading");
    }
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

#include <stdint.h>

// Función para imprimir el offset y el primer valor de cada registro
void print_records(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        size_t offset = 0;

        while (1) {
            size_t size;
            if (fread(&size, sizeof(size_t), 1, file) != 1) {
                // No se pudo leer el tamaño, probablemente llegamos al final del archivo
                break;
            }

            // Imprimir el offset y el primer valor
            printf("Offset: %zu\n", offset);
            printf("Primer valor del registro: ");

            // Leer el registro
            struct Book temp_book;
            if (fread(&temp_book, sizeof(struct Book), 1, file) != 1) {
                perror("Error reading record");
                break;
            }

            // Imprimir el primer valor del registro
            printf("BookID=%d, ISBN=%s, Title=%s, Publisher=%s\n",
                   temp_book.bookID, temp_book.isbn, temp_book.title, temp_book.publisher);

            // Actualizar el offset para el próximo registro
            offset += size;
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
    } else if (strcmp(command, "printInd") == 0) {
        // Imprima la información de los libros almacenados
        print_records(filename);

        //for (int i = 0; i < bookCount; i++) {
        //    printf("Entry #%d\n", i);
        //    printf("    key: #%d\n", books[i].bookID);
        //    printf("    offset: #%zu\n", i * sizeof(struct Book));
        //}
    } else if (strcmp(command, "exit") == 0) {
        // Informe al usuario que el programa va a salir
        printf("Exiting the program.\n");
    } else {
        // Maneje comandos no reconocidos
        printf("Unrecognized command: %s\n", command);
    }
}

// Función principal del bucle
void loop(const char *ordering_strategy, const char *filename) {
    read_books_from_file(filename);

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

        /** Check if the user entered the "exit" command **/
        if (strcmp(command_buffer, "exit") == 0) {
            // Liberar memoria antes de salir
            for (int i = 0; i < bookCount; i++) {
                free_book(&books[i]);
            }
            break; /** Exit the loop **/
        }
    }
}
