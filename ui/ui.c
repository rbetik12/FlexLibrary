#include <ui.h>
#include <ncurses.h>
#include <ui_defines.h>
#include <books.h>
#include <books_defines.h>
#include <form.h>

extern WINDOW* win_book_list;
extern WINDOW* win_filters;
extern WINDOW* win_book_info;
extern bool bookSearchFilters[4];
extern int page_number;
extern int page_size;
extern int book_cursor_pos;
extern book* books[];

void print_book_info() {
    book* selected_book;
    wclear(win_book_info);
    selected_book = books[book_cursor_pos + (page_number * page_size)];
    if (selected_book) {
        wprintw(win_book_info, "Title: %s\n", selected_book->title);
        wprintw(win_book_info, "Authors: %s\n", selected_book->authors);
        wprintw(win_book_info, "Annotation: %s\n", selected_book->annotation);
        wprintw(win_book_info, "Tags: %s\n", selected_book->tags);
    }
    wrefresh(win_book_info);
}

void print_book_list_page() {
    wclear(win_book_list);
    int rowNum = 0;
    for (int i = page_number * page_size; i < page_number * page_size + page_size; i++) {
        if (books[i] != NULL) {
            if (rowNum == book_cursor_pos) {
                wprintw(win_book_list, ">");
                print_book_info();
            }
            wprintw(win_book_list, "%s\n", books[i]->title);
        }
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

extern WINDOW* win_book_list_border;
extern WINDOW* win_book_info_border;
extern WINDOW* win_edit_form;
extern WINDOW* win_form;
extern FIELD* fields[3];

void draw_main_ui() {
    box(win_book_list_border, 0, 0);
    box(win_book_info_border, 0, 0);
    set_field_buffer(fields[1], 0, ">");

    mvprintw(0, COLS - 48, "[Get book F1] [Return book F2] [Add new book F3]");
    refresh();
    wrefresh(win_form);
    wrefresh(win_book_list_border);
    wrefresh(win_book_info_border);
    wrefresh(win_book_list);
    wrefresh(win_filters);
    print_filters();
    print_book_list_page();
}

extern bool open_edit_form;
FORM* edit_form;
FORM* form;

void close_edit_menu() {
    open_edit_form = false;
    unpost_form(edit_form);
    post_form(form);
    form_driver(edit_form, REQ_VALIDATION);
    wclear(stdscr);
    draw_main_ui();
}