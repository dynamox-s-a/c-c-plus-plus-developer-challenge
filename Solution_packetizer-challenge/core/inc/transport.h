#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stddef.h>
#include <stdint.h>


typedef struct
{
    int socket_fd;
    int connection_fd;
} Transport;


int transport_server_init(Transport* transport, uint16_t port);
int transport_accept(Transport* transport);
int transport_client_connect(Transport* transport, const char* ip, uint16_t port);
int transport_send(Transport* transport, const uint8_t* buffer, size_t length);
int transport_receive(Transport* transport, uint8_t* buffer, size_t max_size);
void transport_close(Transport* transport);


#endif