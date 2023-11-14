#include "utils.h"

bool is_empty(char *str) {
    return strlen(str) == 0 || strspn(str, " ") == strlen(str);
}

void write_choice(char * choice, char ***choices, int choice_index, int max_length) {
    int t = (int)strlen(choice)+1;
    t = MIN(t, max_length);
    strncpy((*choices)[choice_index], choice, (size_t)t);
}
