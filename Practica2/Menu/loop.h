/*
* Created by roberto on 30/4/21.
*/


#ifndef NCOURSES_LOOP_H
#define NCOURSES_LOOP_H
#include "windows.h"
#include "search.h"
#include "lbpass.h"

/* menu options */
#define SEARCH 0
#define BPASS 1
#define QUIT 2

#define FOCUS_LEFT 0
#define FOCUS_RIGHT 1

void loop(_Windows * windows, _Menus  *menus,
          _Forms *forms, _Panels *panels);

#endif /* NCOURSES_LOOP_H */
