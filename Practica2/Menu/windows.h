/*
* Created by roberto on 30/4/21.
*/

#ifndef NCOURSES_WINDOWS_H
#define NCOURSES_WINDOWS_H
#include <ncurses.h>
#include <assert.h>
#include <form.h>
#include <menu.h>
#include <panel.h>
#include <stdlib.h>
#define DEBUG 1
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct _Windows {
    WINDOW *menu_win,
            *form_search_win,
            *form_bpass_win,
            *out_win,
            *main_win,
            *msg_win;
    int height_menu_win;
    int width_form_win;
    int terminal_ncols;
    int terminal_nrows;
    int rows_out_win;
    int cols_out_win;
    char * menu_title;
    char * form_search_title;
    char * form_bpass_title;
    char * out_title;
    char * msg_title;
} _Windows;

typedef  struct _Panels{
    PANEL *search_panel,
          *bpass_panel;
} _Panels;

typedef struct _Menus{
    MENU *menu;
    char **choices;
    int no_items;
    ITEM **menuitems;
    char **out_win_choices;
} _Menus;

typedef struct _Forms{
    FORM * search_form,
            * bpass_form;
    char ** choices_search_form;
    char ** choices_bpass_form;
    int no_items_search;
    int no_items_bpass;
    FIELD **search_form_items;
    FIELD **bpass_form_items;
} _Forms;

void _initsrc(_Windows *windows,
              _Menus *menus,
              _Forms *forms,
              _Panels *panels);

void print_menu(WINDOW *menu_win,
                char ** choices,
                int menuitems,
                int highlight,
                char * title);
void print_out(WINDOW *win,
                char ** choices,
                int menuitems,
                int highlight,
                char * title);

void write_msg(WINDOW * win, char * msg, int y, int x, char * title);

#endif /* NCOURSES_WINDOWS_H */
