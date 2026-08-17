#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: ./filename <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    int main_socket, new_socket, rw_bytes;
    char *reply = "world";
    char buffer[BUFF_SIZE] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((main_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Socket creation error\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(main_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Binding Failed.");
        exit(1);
    }

    if (listen(main_socket, 1) < 0)
    {
        perror("Listening Failed.");
        exit(1);
    }

    while (1)
    {
        if ((new_socket = accept(main_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
        {
            perror("Accept Failed.");
            exit(1);
        }

        rw_bytes = read(new_socket, buffer, BUFF_SIZE);
        if (rw_bytes < 0)
        {
            perror("Read Error");
            exit(1);
        }
        rw_bytes = write(new_socket, reply, strlen(reply));
        if (rw_bytes < 0)
        {
            perror("Write Error");
            exit(1);
        }
        close(new_socket);
    }
    close(main_socket);
    return 0;
}