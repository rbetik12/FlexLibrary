#pragma once
#include <stddef.h>
#include <books.h>
#include <pthread.h>
#include <stdbool.h>

int connect_to_server(short port);
int connect_to_update_server(short port);
int send_data(int socket_fd, void* buffer, size_t data_size);
int read_data(int socket_fd, void* buffer, size_t buffer_size);

int get_all_books(int socket_fd, book* books[]);
int update_book(int socket_fd, book book);
int create_book(int socket_fd, book* book);


typedef struct {
    int update_socket_fd;
    book** books;
    book** books_srch;
    bool* update_req;
    bool* is_search_active;
} client_thread_args;

int create_accept_client_thread(pthread_t* thread, int update_socket_fd, book** books, book** books_srch,
                                    bool* update_req, bool* active_search);