#include <books.h>
#include <stdlib.h>
#include <stdio.h>

void generate_books(book* books[]) {
    for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
        books[i] = NULL;
    }
    for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
        books[i] = calloc(1, sizeof(book));
        books[i]->book_id = i;
        snprintf(books[i]->title, MAX_BOOK_TITLE_LENGTH, "Book%d", i);
        snprintf(books[i]->authors, MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH, "Author1, Author2");
        snprintf(books[i]->tags, MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH, "Cool");
        snprintf(books[i]->annotation, MAX_BOOK_ANNOTATION_LENGTH, "I Billy Harrington!");
    }
}

int init_books(book* books[]) {
    for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
        books[i] = NULL;
    }
    return 0;
}