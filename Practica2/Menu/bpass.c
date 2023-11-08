/*
* Created by roberto on 3/5/21.
*/
#include "lbpass.h"
void    results_bpass(/*@unused@*/ char * bookID,
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows)
/**here you need to do your query and fill the choices array of strings
*
* @param bookID  form field bookId
* @param n_choices fill this with the number of results
* @param choices fill this with the actual results
* @param max_length output win maximum width
* @param max_rows output win maximum number of rows
*/

{
    int i=0;
    int t=0;

    char *query_result_set[]={
            "A: Hi, what's your name?",
            "B: I'm Jenny. You?",
            "A: Oh I'm Akiko. It's great to meet you. So where are you from?",
            "B: I'm from New York. I am in Tokyo for a 10-day work trip.",
            "A: How do you like Japan so far?",
            "B: Oh my gosh, I never imagined the food would be this great and I'm having a blast.",
            "A: Cool!  Are you getting a lot of time to explore outside of work?"
    };
    *n_choices = (int) (sizeof(query_result_set) / sizeof(query_result_set[0]));
    max_rows = MIN(*n_choices, max_rows);
    for (i = 0 ; i < max_rows ; i++) {
        t = (int)strlen(query_result_set[i])+1;
        t = MIN(t, max_length);
        strncpy((*choices)[i], query_result_set[i], (size_t)t);
    }
}

