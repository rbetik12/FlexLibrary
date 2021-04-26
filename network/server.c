#include <server.h>
#include <network_defines.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int server_fd, opt;
int freeSocketNum = 0;
int clientSockets[MAX_NETWORK_CLIENTS_AMOUNT];
int new_socket;
struct sockaddr_in address;

int init_server(short port) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Can't create server socket!");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_NETWORK_CLIENTS_AMOUNT) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr*) &address,
                             (socklen_t*) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    int valread;
    char buffer[1024] = {0};
    char* hello = "Hello from server";
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    close(new_socket);
    close(server_fd);
    return 0;
}
