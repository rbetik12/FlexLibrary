#pragma once

#include <stdint.h>
#include <books_defines.h>

typedef struct book {
    char title[MAX_BOOK_TITLE_LENGTH];
    char authors[MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH];
    char annotation[MAX_BOOK_ANNOTATION_LENGTH];
    char tags[MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH];
} book;

void generate_books(book* books[]);
int init_books(book* books[]);
