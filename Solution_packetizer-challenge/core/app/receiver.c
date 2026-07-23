#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <packetizer.h>
#include <transport.h>
#include <fragmenter.h>


int main(void)
{
    Transport transport;

    if (transport_server_init(&transport, 5000) != 0)
    {
        printf("Failed to create server.\n");
        return 1;
    }

    if (transport_accept(&transport) != 0)
    {
        printf("Accept failed.\n");
        return 1;
    }

    Packetizer packetizer;
    packetizer_init(&packetizer, &transport);

    uint8_t message[MAX_MESSAGE_SIZE];
    size_t message_size;

    while (1)
    {
        if (!packetizer_receive(&packetizer, message, &message_size))
            continue;

        fwrite(message, 1, message_size, stdout);
        printf("\n");
    }

    transport_close(&transport);

    return 0;
}