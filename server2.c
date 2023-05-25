#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_INT_SIZE 256

int main(int argc, char *argv[])
{

    int sockfd, ret, port_number, newsockfd;
    struct sockaddr_in server_address, new_socket_address;

    socklen_t socket_address_size;

    char buffer[MAX_INT_SIZE];
    pid_t childpid;
    if (argc < 2)
    {
        fprintf(stderr, "No Port\n");
        exit(1);
    }

    port_number = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[ERROR] connection faild.\n");
        exit(1);
    }

    bzero((char *)&server_address, sizeof(server_address));
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("[ERROR] binding failed.\n");
        exit(1);
    }

    if (listen(sockfd, 4) < 0)
    {
        printf("[ERROR] listening failed.\n");
        exit(1);
    }
    else
    {
        printf("Listening....\n");
    }

    while (1)
    {
        int clilen = sizeof(server_address);
        newsockfd = accept(sockfd, (struct sockaddr *)&new_socket_address, (socklen_t *)&clilen);
        if (newsockfd < 0)
        {
            fprintf(stderr, "ERROR accepting.\n");
            exit(1);
        }
        else
        {
            printf("Connected to %s: %d\n", inet_ntoa(new_socket_address.sin_addr), ntohs(new_socket_address.sin_port));
        }
        if ((childpid = fork()) == 0)
        {
            close(sockfd);
            while (1)
            {
                recv(newsockfd, buffer, sizeof(buffer), 0);

                if (buffer[0] == '\0')
                {
                    printf("Client disconnected...\n");
                    exit(0);
                }
                else
                {
                    printf("Client %d message : %s\n", ntohs(new_socket_address.sin_port), buffer);
                    int flag = 0;
                    int num1 = 0, num2 = 0;
                    int c, j;
                    for (int i = 0; i < strlen(buffer) - 1; i++)
                    {
                        if (buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/')
                        {
                            flag = 1;
                            j = i;
                        }
                        else
                        {
                            if (flag == 0)
                            {
                                num1 = (10 * num1) + (buffer[i] - '0');
                            }
                            else if (flag != 0)
                            {
                                num2 = (10 * num2) + (buffer[i] - '0');
                            }
                        }
                    }

                    int result;

                    if (buffer[j] == '+')
                    {
                        result = num1 + num2;
                    }
                    else if (buffer[j] == '-')
                    {
                        result = num1 - num2;
                    }
                    else if (buffer[j] == '/')
                    {
                        result = num1 / num2;
                    }
                    else if (buffer[j] == '*')
                    {
                        result = num1 * num2;
                    }

                    if (result < 0)
                    {
                        perror("[ERROR] writing failed.\n");
                    }
                    printf("Sending reply to client %d:%i\n", ntohs(new_socket_address.sin_port), result);
                    bzero(buffer, MAX_INT_SIZE);
                    sprintf(buffer, "%d", result);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
            }
        }
    }

    if (read(newsockfd, buffer, sizeof(buffer)) <= 0)
    {
        printf("closing server..\n");
    }

    close(newsockfd);
    return 0;
}