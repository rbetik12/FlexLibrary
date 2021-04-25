#include <ui.h>
#include <ncurses.h>
#include <ui_defines.h>
#include <books.h>
#include <books_defines.h>
#include <assert.h>
#include <string.h>

extern WINDOW* win_book_list;
extern WINDOW* win_filters;
extern bool bookSearchFilters[4];
extern int page_number;
extern int page_size;
extern int book_cursor_pos;
extern book* books;

void print_book_list_page() {
    wclear(win_book_list);
    int rowNum = 0;
    int tmp0 = page_size;
    int tmp1 = page_number;
    assert(strcmp(books[0].title, "Book0\n") == 0);
    for (int i = page_number * page_size; i < page_number * page_size + page_size; i++) {
        if (rowNum == book_cursor_pos) {
            wprintw(win_book_list, ">");
        }
        wprintw(win_book_list, books[i].title);
        rowNum += 1;
    }
    wrefresh(win_book_list);
}

void books_page_down() {
    page_number += 1;
    if (page_number >= MAX_BOOKS_AMOUNT / page_size) {
        page_number = (MAX_BOOKS_AMOUNT / page_size) - 1;
    }
    print_book_list_page();
}

void books_page_up() {
    page_number -= 1;
    if (page_number < 0) {
        page_number = 0;
    }
    print_book_list_page();
}

void print_filters() {
    wclear(win_filters);
    wprintw(win_filters, "filter by %s title (F4), %s author (F5), %s annotation (F6), %s tags (F7)",
            bookSearchFilters[SEARCH_IN_TITLE] ? "[X]" : "[]",
            bookSearchFilters[SEARCH_IN_AUTHOR] ? "[X]" : "[]",
            bookSearchFilters[SEARCH_IN_ANNOTATION] ? "[X]" : "[]",
            bookSearchFilters[SEARCH_IN_TAGS] ? "[X]" : "[]"
    );
    wrefresh(win_filters);
}

void init_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void config_windows() {
    immedok(win_book_list, TRUE);
}