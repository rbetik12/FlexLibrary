#pragma once

#include <stdint.h>

typedef struct book {
    char* title;
} book;

book* generate_books(uint32_t amount);
