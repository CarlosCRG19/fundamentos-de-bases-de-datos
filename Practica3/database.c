#include "database.h"
#include "stdlib.h"

Database* Database_new(enum OrderingStrategy ordering_strategy, char *filename) {
    Database* d = (Database*)malloc(sizeof(Database));
    d->filename = filename;
    d->ordering_strategy = ordering_strategy;

    d->size = 0;
    d->records = NULL;

    return d;
}
