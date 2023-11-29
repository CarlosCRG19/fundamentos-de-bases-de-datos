#include "loop.h"
#include <stdlib.h>

// Estructura para almacenar información sobre un libro
struct Book {
    int bookID;
    char *isbn;
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
void write_book_to_file(const char *filename, struct Book *book, int size) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(book, size, 1, file);
        fclose(file);
    } else {
        perror("Error opening file for writing");
    }
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

// Función para procesar comandos
void process_command(const char *command, const char *ordering_strategy, const char *filename) {
    if (strncmp(command, "add", 3) == 0) {
        // Parse los datos del libro desde el comando
        sscanf(command, "add %d|%[^|]|%[^|]|%[^\r]", &books[bookCount].bookID, books[bookCount].isbn, books[bookCount].title, books[bookCount].publisher);
        int size = sizeof(&books[bookCount].bookID) + sizeof(&books[bookCount].isbn) + sizeof(&books[bookCount].title) + sizeof(&books[bookCount].publisher);
        write_book_to_file(filename, &books[bookCount], size);

        // Imprima un mensaje indicando que el libro ha sido agregado
        printf("Record with BookID=%d has been added to the database\n", books[bookCount].bookID);
        printf("size: #%d\n", size);
        printf(&books[bookCount].title);
        printf("a2: %s\n", books[bookCount].title);
        printf("exit\n");

        // Incremente el contador de libros
        bookCount++;

    } else if (strcmp(command, "printInd") == 0) {
        // Imprima la información de los libros almacenados
        for (int i = 0; i < bookCount; i++) {
            printf("Entry #%d\n", i);
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
