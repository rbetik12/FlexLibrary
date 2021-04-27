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

int opt;
int freeSocketNum = 0;
int clientSockets[MAX_NETWORK_CLIENTS_AMOUNT] = {0};
struct sockaddr_in address;
pthread_t acceptThread;
bool shutdown_server = false;

typedef struct {
    int server_fd;
} accept_thread_args;

void* accept_thread(void* _args) {
    int addr_len = sizeof(address);
    accept_thread_args* args = (accept_thread_args*) _args;

    while (!shutdown_server) {
        if ((clientSockets[freeSocketNum] = accept(args->server_fd, (struct sockaddr*) &address, (socklen_t*) &addr_len)) < 0) {
            perror("accept");
        }
        if (freeSocketNum == MAX_NETWORK_CLIENTS_AMOUNT - 1) {
            perror("max client exceeded");
        }
        else {
            freeSocketNum += 1;
        }
    }

    free(args);
    return NULL;
}

int create_accept_thread(int server_fd) {
    accept_thread_args* args = malloc(sizeof(accept_thread_args));
    args->server_fd = server_fd;

    if (pthread_create(&acceptThread, NULL, accept_thread, (void*)args)) {
        perror("Can't create accept server thread");
        return -1;
    }
    return 0;
}

int init_server(short port) {
    int server_fd = 0;
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

    return server_fd;
}

int start_server(int server_fd) {
    if (server_fd <= 0) {
        return -1;
    }
    int read_amount = 0;
    char echo_buffer[1024] = {0};

    while (!shutdown_server) {
        for (int socket_num = 0; socket_num < MAX_NETWORK_CLIENTS_AMOUNT; socket_num++) {
            if (clientSockets[socket_num] <= 0) {
                continue;
            }
            read_amount = read(clientSockets[socket_num], echo_buffer, sizeof(echo_buffer));
            if (read_amount > 0) {
                send(clientSockets[socket_num], echo_buffer, strlen(echo_buffer), 0);
            }
        }
    }

    return 0;
}
