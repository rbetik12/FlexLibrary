#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <form.h>
#include <books.h>
#include <ui_defines.h>
#include <ui.h>
#include <books_defines.h>
#include <assert.h>
#include <server.h>
#include <client.h>
#include <ctype.h>

FORM* form;
FORM* edit_form;
FIELD* fields[3];
FIELD* edit_field[2];
WINDOW* win_book_list_border,
        * win_book_info_border,
        * win_form,
        * win_filters,
        * win_book_list,
        * win_book_info,
        * win_edit_form;
int page_number = 0;
int page_size = 0;
int book_cursor_pos = 0;
int client_socket = 0;
int server_fd = 0;
book* books;
bool bookSearchFilters[4] = {false, false, false, false};
bool open_edit_form = false;
bool running = true;

typedef enum {
    TITLE,
    AUTHORS,
    ANNOTATION,
    TAGS,
    NONE
} EDIT_FIELD;

EDIT_FIELD current_edit_field = NONE;

char* trim_whitespaces(char* str) {
    char* end;

    while (isspace(*str))
        str++;
    if (*str == 0)
        return str;

    end = str + strnlen(str, 128) - 1;

    while (end > str && isspace(*end))
        end--;

    *(end + 1) = '\0';
    return str;
}

void process_input(int ch) {
    MEVENT event;
    switch (ch) {
        case KEY_ENTER:
            if (open_edit_form) {
                close_edit_menu();
            }
            break;

        case 48:
            if (!open_edit_form) {
                open_edit_form = true;
                unpost_form(form);
                post_form(edit_form);
                set_current_field(edit_form, edit_field[0]);
                wclear(stdscr);
                attron(A_REVERSE);
                int ui_row_offset = COLS / 2 - strlen("[Title F1] [Authors F2] [Annotation F3] [Close 0]");
                if (ui_row_offset < 0) {
                    ui_row_offset = -ui_row_offset;
                }
                mvwprintw(stdscr, 0, COLS / 2 - strlen("Editing book"), "Editing book");
                mvwprintw(stdscr, 1, ui_row_offset, "[Title F1] [Authors F2] [Annotation F3] [Tags F4] [Close 0]");
                attroff(A_REVERSE);
                refresh();
                wrefresh(win_edit_form);
            } else if (open_edit_form) {
                book* cur_book = books + (page_number * page_size + book_cursor_pos);
                form_driver(edit_form, REQ_VALIDATION);
                char* field_buffer_value = field_buffer(edit_field[0], 0);
                field_buffer_value = trim_whitespaces(field_buffer_value);

                switch (current_edit_field) {
                    case TITLE:
                        memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                        break;
                    case AUTHORS:
                        memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                        break;
                    case ANNOTATION:
                        memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                        break;
                    case TAGS:
                        memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                        break;
                    default:
                        break;
                }

                current_edit_field = NONE;
                close_edit_menu();
            }
        case KEY_MOUSE:
            if (getmouse(&event) == OK) {
            }
            wrefresh(win_book_list_border);
            break;

        case KEY_F(1):
            if (open_edit_form) {
                current_edit_field = TITLE;
                move(0, 0);
                clrtoeol();
                book cur_book = books[page_number * page_size + book_cursor_pos];
                set_field_buffer(edit_field[0], 0, cur_book.title);
                attron(A_REVERSE);
                mvwprintw(stdscr, 0, COLS / 2 - strlen("Editing book title"), "Editing book title");
                attroff(A_REVERSE);
                refresh();
                wrefresh(win_edit_form);
            }
            break;

        case KEY_F(2):
            if (open_edit_form) {
                current_edit_field = AUTHORS;
                move(0, 0);
                clrtoeol();
                book cur_book = books[page_number * page_size + book_cursor_pos];
                set_field_buffer(edit_field[0], 0, cur_book.authors);
                attron(A_REVERSE);
                mvwprintw(stdscr, 0, COLS / 2 - strlen("Editing book authors"), "Editing book authors");
                attroff(A_REVERSE);
                refresh();
                wrefresh(win_edit_form);
            }
            break;

        case KEY_F(3):
            if (open_edit_form) {
                current_edit_field = ANNOTATION;
                move(0, 0);
                clrtoeol();
                book cur_book = books[page_number * page_size + book_cursor_pos];
                set_field_buffer(edit_field[0], 0, cur_book.annotation);
                attron(A_REVERSE);
                mvwprintw(stdscr, 0, COLS / 2 - strlen("Editing book annotation"), "Editing book annotation");
                attroff(A_REVERSE);
                refresh();
                wrefresh(win_edit_form);
            }
            break;

        case KEY_F(4):
            if (open_edit_form) {
                current_edit_field = TAGS;
                move(0, 0);
                clrtoeol();
                book cur_book = books[page_number * page_size + book_cursor_pos];
                set_field_buffer(edit_field[0], 0, cur_book.tags);
                attron(A_REVERSE);
                mvwprintw(stdscr, 0, COLS / 2 - strlen("Editing book tags"), "Editing book tags");
                attroff(A_REVERSE);
                refresh();
                wrefresh(win_edit_form);
            } else {
                bookSearchFilters[SEARCH_IN_TITLE] = !bookSearchFilters[SEARCH_IN_TITLE];
                print_filters();
            }
            break;

        case KEY_F(5):
            bookSearchFilters[SEARCH_IN_AUTHOR] = !bookSearchFilters[SEARCH_IN_AUTHOR];
            print_filters();
            break;

        case KEY_F(6):
            bookSearchFilters[SEARCH_IN_ANNOTATION] = !bookSearchFilters[SEARCH_IN_ANNOTATION];
            print_filters();
            break;

        case KEY_F(7):
            bookSearchFilters[SEARCH_IN_TAGS] = !bookSearchFilters[SEARCH_IN_TAGS];
            print_filters();
            break;

        case KEY_LEFT:
            if (open_edit_form) {
                form_driver(edit_form, REQ_PREV_CHAR);
            } else {
                form_driver(form, REQ_PREV_CHAR);
            }
            break;

        case KEY_RIGHT:
            if (open_edit_form) {
                form_driver(edit_form, REQ_NEXT_CHAR);
            } else {
                form_driver(form, REQ_NEXT_CHAR);
            }
            break;

        case KEY_UP:
            if (open_edit_form) {}
            else {
                book_cursor_pos -= 1;
                if (book_cursor_pos < 0) {
                    book_cursor_pos = page_size - 1;
                    books_page_up();
                }
                print_book_list_page();
                print_book_info();
            }
            break;

        case KEY_DOWN:
            if (open_edit_form) {}
            else {
                book_cursor_pos += 1;
                if (book_cursor_pos >= page_size) {
                    book_cursor_pos = 0;
                    books_page_down();
                }
                print_book_list_page();
                print_book_info();
            }
            break;

        case KEY_NPAGE:
            books_page_down();
            break;

        case KEY_PPAGE:
            books_page_up();
            break;

        case KEY_BACKSPACE:
        case 127:
            if (open_edit_form) {
                form_driver(edit_form, REQ_DEL_PREV);
            } else {
                form_driver(form, REQ_DEL_PREV);
            }
            break;

        case KEY_DC:
            if (open_edit_form) {
                form_driver(edit_form, REQ_DEL_CHAR);
            } else {
                form_driver(form, REQ_DEL_CHAR);
            }
            break;

        default:
            if (open_edit_form) {
                form_driver(edit_form, ch);
            } else {
                form_driver(form, ch);
            }
            break;
    }

    wrefresh(win_form);
}

int main(int argc, char* argv[]) {
//    if (argc < 3) {
//        puts("FlexLibrary --client | --server port");
//        exit(EXIT_FAILURE);
//    }
//    char* end;
//    short port = strtol(argv[2], &end, 10);
//    if (strcmp(argv[1], "--client") == 0) {
//        client_socket = connect_to_server(port);
//        if (client_socket <= 0) {
//            puts("Can't connect to server!\n");
//            exit(EXIT_FAILURE);
//        }
//
//    }
//    else if (strcmp(argv[1], "--server") == 0) {
//        server_fd = init_server(port);
//        if (server_fd <= 0) {
//            puts("Can't start server!\n");
//            exit(EXIT_FAILURE);
//        }
//        start_server(server_fd);
//    }
    int ch;
    books = generate_books(MAX_BOOKS_AMOUNT);
    assert(books != NULL);

    init_ncurses();
    win_book_list_border = newwin(LINES - 2, COLS / 3, 1, 0);
    win_book_info_border = newwin(LINES - 2, COLS * 2 / 3, 1, COLS / 3);
    win_book_list = newwin(LINES - 4, COLS / 3 - 2, 2, 1);
    win_book_info = newwin(LINES - 4, (COLS * 2 / 3) - 1, 2, (COLS / 3) + 1);
    win_filters = newwin(1, COLS, LINES - 1, 0);
    win_form = derwin(stdscr, 1, COLS, 0, 0);

    win_edit_form = derwin(stdscr, 4, COLS, 0, 0);
    config_windows();

    page_size = LINES - 4;

    box(win_book_list_border, 0, 0);
    box(win_book_info_border, 0, 0);
    box(win_edit_form, 0, 0);
    fields[0] = new_field(1, 25, 0, 3, 0, 0);
    fields[1] = new_field(1, 2, 0, 0, 0, 0);
    fields[2] = NULL;

    edit_field[0] = new_field(3, COLS - 1, 2, 0, 0, 0);
    edit_field[1] = NULL;

    set_field_buffer(fields[0], 0, "");
    set_field_buffer(fields[1], 0, ">");

    set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(edit_field[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_back(edit_field[0], A_UNDERLINE);

    form = new_form(fields);
    set_form_win(form, stdscr);
    set_form_sub(form, derwin(win_form, 18, 76, 1, 1));
    post_form(form);

    edit_form = new_form(edit_field);
    set_form_win(edit_form, stdscr);
    set_form_sub(edit_form, derwin(win_edit_form, 18, 76, 1, 1));

    mvprintw(0, COLS - 48, "[Get book F1] [Return book F2] [Add new book F3]");
    mvwprintw(win_book_info_border, 0, 0, "Press 0 to edit this book");

    refresh();
    wrefresh(win_form);
    wrefresh(win_book_list_border);
    wrefresh(win_book_info_border);
    wrefresh(win_book_list);
    wrefresh(win_filters);

    print_filters();
    print_book_list_page();

    while (running) {
        ch = getch();
        process_input(ch);
    }

    unpost_form(form);
    free_form(form);
    free_field(fields[0]);
    free_field(fields[1]);
    unpost_form(edit_form);
    free_form(edit_form);
    free_field(edit_field[0]);
    delwin(win_form);
    delwin(win_filters);
    delwin(win_book_list);
    delwin(win_book_info);
    delwin(win_book_list_border);
    delwin(win_book_info_border);
    delwin(win_edit_form);
    endwin();
    free(books);
    return 0;
}