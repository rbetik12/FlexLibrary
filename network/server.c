#include <server.h>
#include <network_defines.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <books.h>
#include <fcntl.h>
#include <errno.h>

int opt;
int free_socket_num = 0;
int client_sockets[MAX_NETWORK_CLIENTS_AMOUNT] = {0};
struct sockaddr_in address;
pthread_t acceptThread;
bool shutdown_server = false;
book* books[MAX_BOOKS_AMOUNT];

typedef struct {
    int server_fd;
} accept_thread_args;

void* accept_thread(void* _args) {
    int addr_len = sizeof(address);
    accept_thread_args* args = (accept_thread_args*) _args;
    int socket_fd = 0;

    while (!shutdown_server) {
        if ((client_sockets[free_socket_num] = accept(args->server_fd, (struct sockaddr*) &address, (socklen_t*) &addr_len)) < 0) {
            perror("accept");
        }
        if (free_socket_num == MAX_NETWORK_CLIENTS_AMOUNT - 1) {
            perror("max client exceeded");
        }
        else {
            free_socket_num += 1;
        }
    }

    free(args);
    return NULL;
}

int create_accept_thread(int server_fd) {
    accept_thread_args* args = malloc(sizeof(accept_thread_args));
    args->server_fd = server_fd;

    if (pthread_create(&acceptThread, NULL, accept_thread, (void*) args)) {
        perror("Can't create accept server thread");
        return -1;
    }
    return 0;
}

int init_server(short port) {
    int server_fd = 0;
    int flags;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Can't create server socket!");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    if (listen(server_fd, MAX_NETWORK_CLIENTS_AMOUNT) < 0) {
        perror("listen");
        return -1;
    }

    if (create_accept_thread(server_fd)) {
        perror("accept thread");
        return -1;
    }

    generate_books(books);

    return server_fd;
}

void server_tick(int command, int socket_num) {
    switch (command) {
        case GET_ALL:
            printf("Get all\n");
            for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
                if (books[i] == NULL) {
                    send(client_sockets[socket_num], "eof1", strlen("eof1") + 1, 0);
                    break;
                }
                send(client_sockets[socket_num], books[i], sizeof(book), 0);
            }
            send(client_sockets[socket_num], "eof", strlen("eof") + 1, 0);
            break;
        case UPDATE_BOOK: {
            book book;
            read(client_sockets[socket_num], &book, sizeof(book));
            printf("Updating book\n");
            for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
                if (books[i]->book_id == book.book_id) {
                    if (books[i]) {
                        memcpy(books[i], &book, sizeof(book));
                    }
                }
            }
            break;
        }
        case NEW_BOOK: {
            book book;
            read(client_sockets[socket_num], &book, sizeof(book));
            printf("New book\n");
            for (int i = 0; i < MAX_BOOKS_AMOUNT; i++) {
                if (!books[i]) {
                    books[i] = calloc(1, sizeof(book));
                    book.book_id = i;
                    memcpy(books[i], &book, sizeof(book));
                    break;
                }
            }
            break;
        }
        default:
            printf("Unknown command!\n");
    }
}

int start_server(int server_fd) {
    if (server_fd <= 0) {
        return -1;
    }
    int read_amount = 0;
    COMMAND command;
    fd_set read_fds;
    int ret;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    while (!shutdown_server) {
        FD_ZERO(&read_fds);
        for (int socket_num = 0; socket_num < MAX_NETWORK_CLIENTS_AMOUNT; socket_num++) {
            if (client_sockets[socket_num] <= 0) {
                continue;
            }
            FD_SET(client_sockets[socket_num], &read_fds);
//            printf("Updating socket with id: %d\n", client_sockets[socket_num]);
            ret = select(client_sockets[socket_num] + 1, &read_fds, NULL, NULL, &tv);
            if (ret < 0) {
                perror("ret");
            }
            if (ret == 0) {
                continue;
            }
            if (ret > 0) {
                read_amount = read(client_sockets[socket_num], &command, sizeof(command));
                if (read_amount > 0) {
                    server_tick(command, socket_num);
                }
            }
        }
    }

    return 0;
}
