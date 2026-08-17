#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#define BUFF_SIZE 1024
#define MAX_EVENTS 100

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: ./filename <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    int main_socket, new_socket, epoll_fd, event_count, rw_bytes;
    char *reply = "world";
    char buffer[BUFF_SIZE] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct epoll_event event, events[MAX_EVENTS];

    if ((main_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
        perror("Listen Failed.");
        exit(1);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll create fail");
        exit(1);
    }

    event.events = EPOLLIN;
    event.data.fd = main_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, main_socket, &event) == -1)
    {
        perror("epoll ctl server error");
        exit(1);
    }

    while (1)
    {
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (event_count < 0)
        {
            perror("epoll wait failed");
            exit(1);
        }

        for (int i = 0; i < event_count; i++)
        {
            if (events[i].data.fd == main_socket)
            {
                client_len = sizeof(client_addr);
                if ((new_socket = accept(main_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
                {
                    perror("Accept Failed.");
                    exit(1);
                }

                event.events = EPOLLIN;
                event.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1)
                {
                    perror("Epoll ctl error");
                    close(new_socket);
                }
            }
            else
            {
                int client_fd = events[i].data.fd;
                bzero(buffer, BUFF_SIZE);
                rw_bytes = read(client_fd, buffer, BUFF_SIZE);
                if (rw_bytes < 0)
                {
                    perror("Read Error");
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                }
                else if (rw_bytes == 0)
                {
                    printf("Client disconnected\n");
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                }
                else
                {
                    rw_bytes = write(client_fd, reply, strlen(reply));
                    if (rw_bytes < 0)
                    {
                        perror("Write Error");
                        close(client_fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    }
                }
            }
        }
    }
    close(main_socket);
    close(epoll_fd);
    return 0;
}