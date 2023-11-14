#include "utils.h"
#include "odbc.h"
#include "windows.h"

/* Auxiliary function to check if a string is empty */
bool is_empty(char *str) {
    return strlen(str) == 0 || strspn(str, " ") == strlen(str);
}


/* Auxiliary function to write to the choices displayed on out_window */
void write_choice(char * choice, char ***choices, int choice_index, int max_length) {
    int t = (int)strlen(choice)+1;
    t = MIN(t, max_length);
    strncpy((*choices)[choice_index], choice, (size_t)t);
}


void write_error(char **errMsg, char *newErrMsg) {
    if (*errMsg != NULL) {
        free(*errMsg);
    }

    *errMsg = (char *)malloc(strlen(newErrMsg) + 1);

    if (*errMsg != NULL) {
        strcpy(*errMsg, newErrMsg);
    }
}

/* Remove trailing white space characters from string */
void trim_trailing(char * str)
{
    int index, i;

    /* Set default index */
    index = -1;

    /* Find last index of non-white space character */
    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index= i;
        }

        i++;
    }

    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}
