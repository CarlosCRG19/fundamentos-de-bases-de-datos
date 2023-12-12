#ifndef DATABASE_H
#define DATABASE_H

#include "book.h"
#include "enums.h"

typedef struct Database {
    size_t size;
    Book **records;
    char *filename;
    enum OrderingStrategy ordering_strategy;
} Database;

Database* Database_new(enum OrderingStrategy ordering_strategy, char *filename);

#endif /* DATABASE_H */
