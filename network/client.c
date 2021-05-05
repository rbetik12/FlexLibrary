#include <client.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <network_defines.h>
#include <stdlib.h>

int connect_to_server(short port) {
    int socket_fd;
    struct sockaddr_in serv_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return socket_fd;
}

int send_data(int socket_fd, void* buffer, size_t data_size) {
    send(socket_fd, buffer, data_size, 0);
    return 0;
}

static char read_socket_buffer[4096];

int read_data(int socket_fd, void* buffer, size_t buffer_size) {
    int read_amount = 0;
    read_amount = read(socket_fd, read_socket_buffer, sizeof(read_socket_buffer));
    if (read_amount > 0 && read_amount <= buffer_size) {
        memcpy(buffer, read_socket_buffer, read_amount);
    }
    else if (read_amount == 0) {
        return -1;
    }
    return 0;
}

int get_all_books(int socket_fd, book* books[]) {
    int command = GET_ALL;
    int read_amount = 0;

    send(socket_fd, &command, sizeof(command), 0);
    book book;
    int bookIndex = 0;
    while ((read_amount = read(socket_fd, &book, sizeof(book))) == sizeof(book)) {
        books[bookIndex] = calloc(1, sizeof(book));
        memcpy(books[bookIndex], &book, sizeof(book));
        bookIndex += 1;
    }
    printf("%s %d\n", (char*) &book, bookIndex);

    return 0;
}

int update_book(int socket_fd, book book) {
    int command = UPDATE_BOOK;
    send(socket_fd, &command, sizeof(command), 0);
    send(socket_fd, &book, sizeof(book), 0);
    return 0;
}

int create_book(int socket_fd, book* _book) {
    int command = NEW_BOOK;
    send(socket_fd, &command, sizeof(command), 0);
    send(socket_fd, _book, sizeof(book), 0);
    return 0;
}

