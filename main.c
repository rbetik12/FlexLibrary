#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

int RefreshWindows(WINDOW* searchFieldWindow,
                   WINDOW* booksListWindowBorder,
                   WINDOW* booksListWindow,
                   WINDOW* bookInfoWindowBorder,
                   WINDOW* bookInfoWindow,
                   WINDOW* filterWindow) {
    wrefresh(booksListWindowBorder);
    wrefresh(bookInfoWindowBorder);
    wrefresh(bookInfoWindow);
    wrefresh(booksListWindow);
    wrefresh(searchFieldWindow);
    wrefresh(filterWindow);
    return 0;
}

int DeleteWindows(WINDOW* searchFieldWindow,
                  WINDOW* booksListWindowBorder,
                  WINDOW* booksListWindow,
                  WINDOW* bookInfoWindowBorder,
                  WINDOW* bookInfoWindow,
                  WINDOW* filterWindow) {
    delwin(searchFieldWindow);
    delwin(booksListWindow);
    delwin(booksListWindowBorder);
    delwin(bookInfoWindow);
    delwin(bookInfoWindowBorder);
    delwin(filterWindow);
    return 0;
}

int InitWindows(WINDOW** searchFieldWindow,
                WINDOW** booksListWindowBorder,
                WINDOW** booksListWindow,
                WINDOW** bookInfoWindowBorder,
                WINDOW** bookInfoWindow,
                WINDOW** filterWindow) {

    *searchFieldWindow = newwin(1, COLS, 0, 0);
    *booksListWindowBorder = newwin(LINES - 2, COLS / 3, 1, 0);
    *booksListWindow = newwin(LINES - 4, (COLS / 3) - 2, 2, 1);
    *bookInfoWindowBorder = newwin(LINES - 2, COLS * 2 / 3, 1, COLS / 3);
    *bookInfoWindow = newwin(LINES - 4, (COLS * 2 / 3) - 2, 2, (COLS / 3) + 1);
    *filterWindow = newwin(1, COLS, 0, LINES);
    return 0;
}

int main() {
    if (!initscr()) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(1);
    }

    WINDOW* searchFieldWindow = NULL;
    WINDOW* booksListWindowBorder = NULL;
    WINDOW* booksListWindow = NULL;
    WINDOW* bookInfoWindowBorder = NULL;
    WINDOW* bookInfoWindow = NULL;
    WINDOW* filterWindow = NULL;
    InitWindows(&searchFieldWindow, &booksListWindowBorder, &booksListWindow, &bookInfoWindowBorder, &bookInfoWindow, &filterWindow);
    refresh();

    box(booksListWindowBorder, 0, 0);
    wprintw(booksListWindow, "This text mnieqfnifewinfwenoifweinfewniewfinfewni!");
    box(bookInfoWindowBorder, 0, 0);
    wprintw(bookInfoWindow, "Book description feewgegwgewgewgewgewgewgewgewgewegwgewgewgewgewgew!");
    RefreshWindows(searchFieldWindow, booksListWindowBorder, booksListWindow, bookInfoWindowBorder, bookInfoWindow, filterWindow);
    getch();
    DeleteWindows(searchFieldWindow, booksListWindowBorder, booksListWindow, bookInfoWindowBorder, bookInfoWindow, filterWindow);
    endwin();
    return 0;
}
