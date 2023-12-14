#ifndef DATABASE_H
#define DATABASE_H

#include "book.h"
#include "book_index.h"
#include "deleted_book.h"
#include "constants.h"

typedef struct {
    char* data_file;
    char* index_file;
    char* deleted_file;
    int ordering_strategy;
    BookIndexArray* index_array;
    DeletedBookArray* deleted_array;
} Database;

typedef struct {
    int position;
    BookIndex* book_index;
} BookIndexPosition;

/**
 * Creates a new Database object with the given ordering strategy and filename.
 *
 * @param ordering_strategy The strategy for ordering books (BESTFIT or WORSTFIT).
 * @param filename The base filename for the database files.
 * 
 * @return A pointer to the newly created Database object.
 */
Database* Database_new(int ordering_strategy, char *filename);

/**
 * Adds a new book to the database.
 *
 * @param db The Database object.
 * @param new_book Pointer to the Book struct containing book information.
 * 
 * @return OK if successful, or an appropriate error code otherwise.
 */
enum ReturnStatus add_book(Database* db, Book* new_book);

/**
 * Finds the position of a book in the database index using binary search.
 *
 * @param db The Database object.
 * @param bookID The ID of the book to find.
 * 
 * @return A BookIndexPosition struct indicating the position and book_index.
 */
BookIndexPosition find_book(Database* db, int bookID);

/**
 * Retrieves a book from the database using the given BookIndex.
 *
 * @param db The Database object.
 * @param book_index Pointer to the BookIndex struct.
 * 
 * @return A pointer to the retrieved Book or NULL on error.
 */
Book* get_book(Database* db, BookIndex* book_index);

/**
 * Loads the book index from the specified file.
 *
 * @param filename The name of the index file.
 * 
 * @return A pointer to the loaded BookIndexArray.
 */
BookIndexArray* load_index(const char* filename);

/**
 * Saves the book index to the specified file.
 *
 * @param db The Database object.
 * 
 * @return OK if successful, or an appropriate error code otherwise.
 */
enum ReturnStatus save_index(Database* db);

/**
 * Deletes a book from the database.
 *
 * @param db The Database object.
 * @param bookID The ID of the book to delete.
 * 
 * @return OK if successful, or an appropriate error code otherwise.
 */
enum ReturnStatus delete_book(Database* db, int bookID);

/**
 * Saves the deleted book information to the specified file.
 *
 * @param db The Database object.
 * 
 * @return OK if successful, or an appropriate error code otherwise.
 */
enum ReturnStatus save_deleted(Database* db);

/**
 * Loads the deleted book information from the specified file.
 *
 * @param filename The name of the deleted book file.
 * 
 * @return A pointer to the loaded DeletedBookArray.
 */
DeletedBookArray *load_deleted(const char *filename);

/**
 * Frees the memory allocated for a Database and its associated arrays.
 *
 * @param db The Database to be freed.
 */
void free_database(Database *db);

#endif /* DATABASE_H */
