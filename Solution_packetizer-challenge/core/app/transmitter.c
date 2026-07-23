#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <packetizer.h>
#include <transport.h>



int main(void)
{
    char message[] = "Inserindo uma mensagem de teste que é maior do que os limites dos pacotes para fazer a fragmentacao";

    Transport transport;

    if (transport_client_connect(&transport, "127.0.0.1", 5000) != 0)
    {
        printf("Failed to connect.\n");
        return 1;
    }

    Packetizer packetizer;
    packetizer_init(&packetizer, &transport);

    while (1)
    {
        if (packetizer_send(&packetizer, (const uint8_t *)message, strlen(message)))
            printf("Message sent.\n");
        else
            printf("Failed to send message.\n");

        sleep(1);
    }

    transport_close(&transport);

    return 0;
}






// // uint32_t crc = crc32((uint8_t *)msg, strlen(msg));



//     // printf("CRC = %08X\n", crc);
    
//     // uint8_t d[] = {1,2,3,4,5};
//     //p.payload = d;
    
    

//     // printf("%d %d %s %d %d\n", p.start, p.length, p.payload, p.sequence, p.crc);



// /*
// FILE *fp = fopen("image.png", "rb");

// uint8_t buffer[256];

// size_t n = fread(buffer, 1, sizeof(buffer), fp);

// Packet p;

// p.payload = buffer;
// p.length = n;

// */