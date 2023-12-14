#include "commands.h"
#include "database.h"
#include "constants.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Adds a book to the database using the information from the 'add' command.
 *
 * @param db The Database object.
 * @param add_command The 'add' command string containing book information.
 * 
 * @return None
 */
void add(Database* db, const char* add_command) {
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
    enum ReturnStatus res = add_book(db, &new_book);
    if (res == OK) {
        printf("Record with BookID=%d has been added to the database\n", new_book.bookID);
    } else if (res == BOOK_ALREADY_EXISTS){
        printf("Record with BookID=%d exists.\n", new_book.bookID);
    }
}

/**
 * Prints the details of a single book.
 *
 * @param book Pointer to the Book struct containing book information.
 * 
 * @return None
 */
void print_book(Book *book) {
    printf("%d|%s|%s|%s\n", book->bookID, book->isbn, book->title, book->publisher);
}

/**
 * Finds and prints information about a book in the database based on the 'find' command.
 *
 * @param db The Database object.
 * @param find_command The 'find' command string containing bookID information.
 * 
 * @return None
 */
void find(Database* db, const char* find_command) {
    int bookID = atoi(find_command + 5);
    BookIndexPosition bp = find_book(db, bookID);

    if (bp.book_index == NULL) {
        printf("Record with bookId=%d does not exist\n", bookID);
        return;
    }

    Book* found_book = get_book(db, bp.book_index);
    print_book(found_book);
}

/**
 * Prints the information stored in the index array of the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printInd(Database* db) {
    BookIndexArray* index_array = db->index_array;

    for (size_t i = 0; i < index_array->used; ++i) {
        printf("Entry #%lu\n", i);
        printf("    key: #%d\n", index_array->indices[i].bookID);
        printf("    offset: #%ld\n", index_array->indices[i].offset);
        printf("    size: #%ld\n", index_array->indices[i].size);
    }
}

/**
 * Prints the information stored in the deleted array of the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printLst(Database* db) {
    DeletedBookArray* deleted_array = db->deleted_array;

    for (size_t i = 0; i < deleted_array->used; ++i) {
        printf("Entry #%lu\n", i);
        printf("    offset: #%ld\n", deleted_array->books[i].offset);
        printf("    size: #%ld\n", deleted_array->books[i].size);
    }
}

/**
 * Prints the detailed information of all books in the database.
 *
 * @param db The Database object.
 * 
 * @return None
 */
void printRec(Database* db) {
    BookIndexArray* index_array = db->index_array;

    for (size_t i = 0; i < index_array->used; ++i) {
        Book* book = get_book(db, &index_array->indices[i]);
        print_book(book);
    }
}

/**
 * Deletes a book from the database based on the 'del' command.
 *
 * @param db The Database object.
 * @param del_command The 'del' command string containing bookID information.
 * 
 * @return None
 */
void del(Database* db, const char* del_command) {
    int bookID = atoi(del_command + 4);

    /* Call database function to delete a book */
    enum ReturnStatus res = delete_book(db, bookID);
    if (res == OK) {
        printf("Record with BookID=%d has been deleted\n", bookID);
    } else if (res == BOOK_DOESNT_EXISTS){
        printf("Item with key %d does not exist\n", bookID);
    } else {
        printf("lmaooo\n");
    }
}
