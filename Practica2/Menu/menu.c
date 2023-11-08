/*
 Created by roberto on 30/4/21.
 Template to create a set of windows that will display
 a menu, a couple of forms and some space for output
 and mesages
 (see image bellow)

 This file will divide the screen as follows and
 create a menu

 +-------------------------------+ <-- main_win
 |+-----------------------------+|
 ||          win_menu           ||
 |+-----------------------------+|
 |+--------++-------------------+|
 ||        ||                   ||
 ||        ||                   ||
 ||win_form||   win_out         ||
 ||        ||                   ||
 ||        ||                   ||
 |+--------++-------------------+|
 |+-----------------------------+|
 ||          win_messages        ||
 |+-----------------------------+|
 +-------------------------------+

 */
#include <string.h>
#include "lmenu.h" /* menu.h is a ncurses header */

static void init_struct(_Windows *windows, __attribute__((unused)) _Panels *panels,
                 _Menus *menus, _Forms *forms)
/** Functions that initialices windows, menus, forms, etc. Note that the initializacion is
 * only partial. The rest will be done in _initsrc after initializating the curses framework
 *
 * @param windows, pointers to windows
 * @param panels, pointers to panels
 * @param menus, pointers to menus
 * @param forms, pointers to forms
 */
{
    /* windows and panels do not require initialization */
    /* menu and form initialization */
    int i;
    /* labels for menu places in win_menu */
    char *menu_choices[] = {
            "Search",
            "B. Pass",
            "Quit"
    };

    /* labels for forms placed in win_form */
    char *forms_search_choices[] = {
            "From: ",
            "To:   ",
            "Date: "
    };
    char *forms_bpass_choices[] = {
            "book Id: "
    };

    /* Menu: copy menu labels to menu structure */
    menus->no_items = (int)(sizeof(menu_choices) / sizeof(menu_choices[0]));
    menus->choices = (char **) malloc(sizeof(char *) * menus->no_items);
    if(menus->choices == NULL)
         exit(0);

    for (i = 0; i < menus->no_items; i++) {
        (menus->choices)[i] = (char *) malloc(
                sizeof(char *) * sizeof(menu_choices[i]) + 1);
        assert(menus->choices[i] != NULL);
        strcpy((menus->choices)[i], menu_choices[i]);
    }

    /* default windows (menu) size */
    windows->height_menu_win = 3;
    windows->width_form_win = 20;

    /* Menu: copy form labels to form structure */
    forms->no_items_search = (int)(sizeof(forms_search_choices) /
            sizeof(forms_search_choices[0]));
    forms->choices_search_form = (char **) malloc(
            sizeof(char *) * forms->no_items_search);
    for (i = 0; i < forms->no_items_search; i++) {
        (forms->choices_search_form)[i] =
                (char *) malloc(sizeof(char *) *
                sizeof(forms_search_choices[i]) + 1);
        strcpy((forms->choices_search_form)[i],
               forms_search_choices[i]);
    }

    /* Form bpass */
    forms->no_items_bpass = (int)(sizeof(forms_bpass_choices) / sizeof(forms_bpass_choices[0]));
    forms->choices_bpass_form = (char **) malloc(sizeof(char *) * forms->no_items_bpass);
    for (i = 0; i < forms->no_items_bpass; i++) {
        (forms->choices_bpass_form)[i] = (char *) malloc(sizeof(char *) * sizeof(forms_bpass_choices[i]) + 1);
        strcpy((forms->choices_bpass_form)[i], forms_bpass_choices[i]);
    }

    /* store titles for the different windows */
    windows->form_search_title =
            (char *) malloc(sizeof(char *) * sizeof("Form Search")+1);
    strcpy(windows->form_search_title, "Form Search");

    windows->form_bpass_title =
            (char *) malloc(sizeof(char *) * sizeof("Form Bpass")+1);
    strcpy(windows->form_bpass_title, "Form Bpass");

    windows->menu_title =
            (char *) malloc(sizeof(char *) * sizeof("Menu")+1);
    strcpy(windows->menu_title, "Menu");

    windows->out_title =
            (char *) malloc(sizeof(char *) * sizeof("Out")+1);
    strcpy(windows->out_title, "Out");

    windows->msg_title =
            (char *) malloc(sizeof(char *) * sizeof("Msg")+1);
    strcpy(windows->msg_title, "Msg");
}

static void free_struct(_Windows windows, _Panels panels,
                 _Menus menus, _Forms forms)
     /** free memory related with structures windows,
      * panels, forms and menus
      *
      * Note regarding valgrind
      *  valgrind --leak-check=full --show-reachable=yes
While I'm not 100% familiar with the internals of ncurses, I think the
 main problem is that ncurses keeps a lot of state in global variables.
 This state is allocated with initscr at the beginning, but never
 deallocated afterwards. Similar memory “leaks” occur when using standard IO
 functions; a buffer is allocated for stdin, stdout, and stderr on first use
 and this buffer is only deallocated when you close these files.
 I wouldn't worry about them too much.
 by 0x10923E: _initsrc (windows.c:35)

The leaks you should worry about are the “unreachable” leaks because they indicate
 memory that can no longer be released by any means because you forgot all pointers to it.


      * @param windows
      * @param panels
      * @param menus
      * @param forms
      */
{
    int i;
    /* free windows */
    free(windows.msg_title);
    free(windows.out_title);
    free(windows.form_search_title);
    free(windows.form_bpass_title);
    free(windows.menu_title);

    unpost_form(forms.search_form);
    unpost_form(forms.bpass_form); 
    unpost_menu(menus.menu);
    free_form(forms.search_form);
    free_form(forms.bpass_form);
    free_menu(menus.menu);

    for (i = 0; i < menus.no_items; i++){
        free_item((menus.menuitems)[i]);
        free(menus.choices[i]);
    }
    free(menus.choices);
    free(menus.menuitems);

    /* free forms */
    for (i = 0; i < 2*forms.no_items_search+1; i++){
        free_field(forms.search_form_items[i]);
    }
    for (i = 0; i < forms.no_items_search; i++)
        free(forms.choices_search_form[i]);
    free(forms.choices_search_form);
    free(forms.search_form_items);

    for (i = 0; i < forms.no_items_bpass; i++)
        free(forms.choices_bpass_form[i]);
    for (i = 0; i < 2*forms.no_items_bpass+1; i++)
        free_field(forms.bpass_form_items[i]);
    free(forms.choices_bpass_form);
    free(forms.bpass_form_items);

    for (i = 0; i < windows.rows_out_win; i++)
        free(menus.out_win_choices[i]);
    free(menus.out_win_choices);

    /*free panels */
    del_panel(panels.search_panel);
    del_panel(panels.bpass_panel);
    /* free windows */
    delwin(windows.msg_win);
    delwin(windows.out_win);
    delwin(windows.form_bpass_win);
    delwin(windows.form_search_win);
    delwin(windows.menu_win);
    delwin(windows.main_win);

    endwin();
}

int main() {
    _Windows windows;
    _Panels panels;
    _Menus menus;
    _Forms forms;

    /* copy default values to data structures */
    init_struct(&windows, &panels, &menus, &forms);

    /* create windows, menus, panels, etc */
    _initsrc(&windows, &menus, &forms, &panels);

    /* process keyboard */
    loop(&windows, &menus, &forms, &panels);

    /* free memory */
    free_struct(windows, panels, menus, forms);

    return 0;
}
