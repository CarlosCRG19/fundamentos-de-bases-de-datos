#ifndef DATABASE_H
#define DATABASE_H

#include "book.h"

typedef struct Database {
    Book **books;
} Database;

#endif /* DATABASE_H */
