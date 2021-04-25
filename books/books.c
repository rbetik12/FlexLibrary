#include <books.h>
#include <stdlib.h>
#include <stdio.h>

book* generate_books(uint32_t amount) {
    book* books = calloc(amount, sizeof(book));
    for (int i = 0; i < amount; i++) {
        books[i].title = calloc(15, 1);
        snprintf(books[i].title, 15, "Book%d\n", i);
    }
    return books;
}

