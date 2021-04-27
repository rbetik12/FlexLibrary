#pragma once
#include <stddef.h>

int connect_to_server(short port);
int send_data(int socket_fd, void* buffer, size_t data_size);
int read_data(int socket_fd, void* buffer, size_t buffer_size);