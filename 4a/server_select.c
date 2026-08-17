#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>

#define BUFF_SIZE 1024
#define MAX_CLIENTS 100

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: ./filename <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    int main_socket, new_socket, max_sfd, sfd, activity, rw_bytes;
    char *reply = "world";
    char buffer[BUFF_SIZE] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sockets[MAX_CLIENTS];
    fd_set read_fds;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

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
        FD_ZERO(&read_fds);

        FD_SET(main_socket, &read_fds);

        max_sfd = main_socket;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sfd = client_sockets[i];

            if (sfd > 0)
            {
                FD_SET(sfd, &read_fds);
            }

            if (sfd > max_sfd)
            {
                max_sfd = sfd;
            }
        }

        activity = select(max_sfd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR)
        {
            printf("Select Error\n");
        }

        if (FD_ISSET(main_socket, &read_fds))
        {
            if ((new_socket = accept(main_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
            {
                perror("Accept Failed.");
                exit(1);
            }

            int slot_found = 0;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    slot_found = 1;
                    break;
                }
            }
            if (!slot_found)
            {
                close(new_socket);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sfd = client_sockets[i];
            if (FD_ISSET(sfd, &read_fds))
            {
                rw_bytes = read(sfd, buffer, BUFF_SIZE);
                if (rw_bytes <= 0)
                {
                    close(sfd);
                    client_sockets[i] = 0;
                }
                else
                {
                    buffer[rw_bytes] = '\0';
                    rw_bytes = write(sfd, reply, strlen(reply));
                }
            }
        }
    }
    return 0;
}