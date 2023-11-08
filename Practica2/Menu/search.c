/*
* Created by roberto on 3/5/21.
*/
#include "search.h"
void    results_search(char * from, char *to, char *date,
                       int * n_choices, char *** choices,
                       int max_length,
                       int max_rows)
   /**here you need to do your query and fill the choices array of strings
 *
 * @param from form field from
 * @param to form field to
 * @param date form field date
 * @param n_choices fill this with the number of results
 * @param choices fill this with the actual results
 * @param max_length output win maximum width
 * @param max_rows output win maximum number of rows
  */
{
    int i=0;
    int t=0;
    /* 10 commandments from King Jorge Bible */
    char *query_result_set[]={
            "1. Thou shalt have no other gods before me.",
            "2. Thou shalt not make unto thee any graven image,"
            " or any likeness of any thing that is in heaven above,"
            " or that is in the earth beneath, or that is in the water "
            "under the earth.",
            "3. Remember the sabbath day, to keep it holy.",
            "4. Thou shalt not take the name of the Lord thy God in vain.",
            "5. Honour thy father and thy mother.",
            "6. Thou shalt not kill.",
            "7. Thou shalt not commit adultery.",
            "8. Thou shalt not steal.",
            "9. Thou shalt not bear false witness against thy neighbor.",
            "10. Thou shalt not covet thy neighbour's house, thou shalt not"
            " covet thy neighbour's wife, nor his manservant, "
            "nor his maidservant, nor his ox, nor his ass, "
            "nor any thing that is thy neighbour's."
    };
    *n_choices = sizeof(query_result_set) / sizeof(query_result_set[0]);

    max_rows = MIN(*n_choices, max_rows);
    for (i = 0 ; i < max_rows ; i++) {
        t = strlen(query_result_set[i])+1;
        t = MIN(t, max_length);
        strncpy((*choices)[i], query_result_set[i], t);
    }
}

