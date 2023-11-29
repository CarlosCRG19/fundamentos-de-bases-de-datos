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

// Arreglo para almacenar libros
struct Book books[100];  // Asumiendo que el máximo número de libros es 100

// Contador para el número actual de libros
int bookCount = 0;

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
    printf("size: #%zu\n", size);
    printf("title: %s\n", book->title);
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

// Función para procesar comandos
void process_command(const char *command, const char *ordering_strategy, const char *filename) {
    if (strncmp(command, "add", 3) == 0) {
        add_book(command, filename);
    } else if (strcmp(command, "printInd") == 0) {
        // Imprima la información de los libros almacenados
        for (int i = 0; i < bookCount; i++) {
            printf("book #%d\n", i);
            printf("    key: #%d\n", books[i].bookID);
            printf("    offset: #%zu\n", i * sizeof(struct Book));
        }
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
