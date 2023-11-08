/*
 Created by roberto on 30/4/21.
 windows related auxiliary functions
*/
#include "windows.h"


static WINDOW *create_newwin(int height, int width, int starty, int startx,
                      char *title) {
    /** Create a window and draw a title
    *
    * @param height: window  height
    * @param width: window width
    * @param starty: position of left top corner (absolute-y)
    * @param startx: position of left top corner (absolute-x)
    * @param title: window title
    *
    * @return pointer to window structure
    */
    WINDOW *local_win = NULL;
    local_win = newwin(height, width, starty, startx);
    assert(local_win != NULL);
    keypad(local_win, TRUE);
    box(local_win, 0, 0);  /* 0, 0 gives default characters
					            * for the vertical and horizontal
                                * lines			*/
    (void) mvwaddstr(local_win, 0, 2, title);
    (void) wrefresh(local_win);        /* Show that box */
    return local_win;
}

static void make_form(int n_choices,
               FIELD ***items,
               char **choices,
               FORM **form,
               WINDOW *win,
               _Windows *windows)
/** Auxiliary function to create a form
 *
 * @param n_choices: number of items in the menu
 * @param items: list of items in the menu
 * @param choices: labels for the items
 * @param form: store the pointer to the form here
 * @param win: form will be attached to this window
 * @param windows: auxiliary information such a window size
 */
{
    int i = 0;
    int cury = 0, curx = 1;
    FIELD *field;
    /* alloc memory fo fiel, each item will be stored in a field */
    *items = (FIELD **) calloc(2 * n_choices + 1, sizeof(FIELD * ));
    assert(items);
    (*items)[n_choices * 2] = NULL;

    /* place the fields in the window */
    for (i = 0; i < n_choices * 2; i++) {
        (*items)[i] = new_field(1, 7, cury, curx, 0, 0);
        field = (*items)[i];
        assert(field);
        if (i % 2 == 1) { /** odd fields are values */
            cury = cury + 1;
            curx = 1;
            set_field_opts(field, O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
            set_field_back(field, A_UNDERLINE);
        } else {  /** even field are labels */
            set_field_buffer(field, 0, choices[i / 2]);
            set_field_opts(field, O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
            curx = 9;
        }
    }
    /* create form */
    *form = new_form(*items);
    /* attach form to window */
    set_form_win(*form, win);
    set_form_sub(*form,
                 derwin(win, windows->terminal_nrows -
                             2 * windows->height_menu_win - 4,
                        windows->width_form_win - 3, 2, 2));
    post_form(*form);
}

static void create_form(_Windows *windows,
                 __attribute__((unused)) _Menus *menus,
                 _Forms *forms, _Panels *panels)
/** Create form window that may display either
 * a search or a bpass form
 *
 */
{
    /* TODO: think about saving geometry of each create window */
    int n_choices = 0;
    /* create windows */
    windows->form_bpass_win =
            create_newwin(
                    windows->terminal_nrows -
                    2 * windows->height_menu_win - 1, windows->width_form_win,
                    windows->height_menu_win + 0, 0, windows->form_bpass_title);
    windows->form_search_win =
            create_newwin(
                    windows->terminal_nrows -
                    2 * windows->height_menu_win - 1, windows->width_form_win,
                    windows->height_menu_win + 0, 0, windows->form_search_title);

    /* create forms*/
    /* 1) search form */
    n_choices = forms->no_items_search;

    make_form(n_choices,
              &(forms->search_form_items),
              forms->choices_search_form,
              &(forms->search_form),
              windows->form_search_win,
              windows);
    /* 2) bpass form */
    n_choices = forms->no_items_bpass;
    make_form(n_choices,
              &(forms->bpass_form_items),
              forms->choices_bpass_form,
              &(forms->bpass_form),
              windows->form_bpass_win,
              windows);

    /* create panels and attach window to panels.
     * Pannels are used when several windows are
     * placed in the same (x,y) position  creating a stack.
     * Pannel allow to change the top (visible) window easily */
    panels->bpass_panel = new_panel(windows->form_bpass_win);
    panels->search_panel = new_panel(windows->form_search_win);
    update_panels();
    doupdate();  /* panel refresh */

    (void) refresh();
    (void) wrefresh(windows->form_search_win);
    (void) wrefresh(windows->menu_win);
}

static void create_out(_Windows *windows, _Menus *menu)
/** Create output window. This window will show the
 * result of the different quweies using a hand made menu system
 *
 * It also allocs the memory (char **) used to save the text
 * taht will be displayed.
 *
 */
{
    int i = 0;

    windows->rows_out_win =
            windows->terminal_nrows - 2 * windows->height_menu_win - 1;
    windows->cols_out_win =
            windows->terminal_ncols - windows->width_form_win - 1;
    windows->out_win = create_newwin(windows->rows_out_win,
                                     windows->cols_out_win,
                                     windows->height_menu_win + 0,
                                     windows->width_form_win + 1,
                                     windows->out_title);

    menu->out_win_choices =
            (char **) calloc(windows->rows_out_win, sizeof(char *));

    for (i = 0; i < windows->rows_out_win; i++)
        (menu->out_win_choices)[i] =
                (char *) calloc(windows->cols_out_win, sizeof(char *));
}

static void create_msg(_Windows *windows)
/** Creates the  message window. This is an output only window.
 * that does  not interact with users.
 */
{
    windows->msg_win =
            create_newwin(windows->height_menu_win + 1,
                          windows->terminal_ncols,
                          windows->terminal_nrows - windows->height_menu_win - 1,
                          0, windows->msg_title);
}


void write_msg(WINDOW *win, char *msg, int y, int x, char *title)
/** write string msg in  window win, at position (y,x)
*
 * @param win window
 * @param msg message
 * @param x message position (X)
 * @param y message position (Y)
 */
{
    if (x < 0)
        x = 2;
    if (y < 0)
        y = 2;
    (void) wclear(win);
    /* since we cleared the box we need to repaint it */
    (void) box(win, 0, 0);
    (void) mvwaddstr(win, 0, 2, title);
    /* write message */
    (void) mvwaddstr(win, y, x, msg);
    (void) wrefresh(win);
}

static void create_menu(_Windows *windows, _Menus *menus)
/** creates the menu window as well as the menu display in it
 */
{
    size_t i = 0;
    size_t n_choices = 0;
    /* create window */
    windows->menu_win = create_newwin(windows->height_menu_win,
                                      windows->terminal_ncols, 0, 0,
                                      windows->menu_title);
    /* create menu */
    n_choices = menus->no_items;
    menus->menuitems = (ITEM **) calloc(n_choices + 1, sizeof(ITEM * ));
    for (i = 0; i < n_choices; i++) {
        (menus->menuitems)[i] = new_item(menus->choices[i], " ");
        assert((menus->menuitems)[i] != NULL);
    }
    (menus->menuitems)[i] = (ITEM *) NULL;
    menus->menu = new_menu(menus->menuitems);
    assert(menus->menu != NULL && windows->menu_win != NULL);
    keypad(windows->menu_win, TRUE);

    /* menu shape, made it horizontal */
    set_menu_format(menus->menu, 1, n_choices);
    set_current_item(menus->menu, (menus->menuitems)[0]);

    /* "*" will be used to mark selected item */
    set_menu_mark(menus->menu, " * ");

    /* attach menu to window */
    set_menu_win(menus->menu, windows->menu_win);

    /* draw menu in a window overlay */
    set_menu_sub(menus->menu, derwin(windows->menu_win, 1,
                                     windows->terminal_ncols - 2, 1, 1));
    /* write menu to the subwindow*/
    post_menu(menus->menu);
    wrefresh(windows->menu_win);
}


void _initsrc(_Windows *windows,
              _Menus *menus,
              _Forms *forms,
              _Panels *panels)
/** initialize windows
*
* @param windows structure with window information
* @param menus
* @param forms
* @param panels
**/
{
    windows->main_win = initscr();  /* curses call to initialize window */
    keypad(stdscr, TRUE);           /* We get F1, F2 etc..          */
    /* cbreak put the terminal into cbreak() mode.
     * In this mode, characters typed by the user
     * are immediately available to the program,
     * and erase/kill character-processing is not performed */
    (void) cbreak();  /* no waiting for Enter key */
    (void) noecho();  /* set no echoing as you type*/
    /* curses call to find size of window */
    getmaxyx(windows->main_win, windows->terminal_nrows,
             windows->terminal_ncols);
    (void) clear();  /* call to clear screen, send cursor to position (0,0) */
    (void) refresh();  /* curses call to implement all changes since last refresh */

    create_menu(windows, menus);
    create_form(windows, menus, forms, panels);
    create_out(windows, menus);
    create_msg(windows);
}


void print_out(WINDOW *win,
               char **choices,
               int menuitems,
               int highlight,
               char *title)
/** print array of strings choices window win (usually out_win)
 *
 * @param win
 * @param choices list of fields
 * @param menuitems number of fields (maximum number of rows)
 * @param highlight highlight this row (0 -> first row)
 * @param title
 */
{
    int x=0, y=0, i=0;
    x = 2;
    y = 1;
    (void) box(win, 0, 0);
    (void) mvwaddstr(win, 0, 2, title);
    for (i = 0; i < menuitems; ++i) {
        if (highlight == i) /* High light the present choice  */
        {
            (void) wattron(win, A_REVERSE);  /** set reverse attribute on */
            (void) mvwprintw(win, y, x, "%s", choices[i]);
            (void) wattroff(win, A_REVERSE); /** set reverse attribute off */
        } else
            (void) mvwprintw(win, y, x, "%s", choices[i]);
        y += 1;
    }
    (void) wrefresh(win);
}

