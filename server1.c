#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_INT_SIZE 256

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, port_number, c;
    struct sockaddr_in server, client;

    socklen_t client_len;

    char buffer[MAX_INT_SIZE];
    if (argc < 2)
    {
        fprintf(stderr, "No Port\n");
        exit(1);
    }

    // argv[0] and argv[2] used in client
    port_number = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[ERROR] connection failed.\n");
        exit(0);
    }

    // Client address binding
    port_number = atoi(argv[1]);
    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    // Bind
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("[ERROR] bind failed.\n");
        exit(0);
    }

    // Listen
    if (listen(sockfd, 4) < 0)
    {
        puts("[ERROR] line busy!\n");
        exit(0);
    }
    else
    {
        printf("Listening....\n");
    }

    client_len = sizeof(client);
    puts("[INFO] Server started...");
    newsockfd = accept(sockfd, (struct sockaddr *)&client, &client_len);

    if (newsockfd < 0)
    {
        perror("[ERROR] accept failed.");
        exit(0);
    }
    else
    {
        printf("Connected with client socket number: %d \n", newsockfd);
    }

    while (read(newsockfd, buffer, sizeof(buffer)))
    {
        printf("Client socket %d sent message: %s", newsockfd, buffer);
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
        printf("Sending reply:%i\n", result);
        bzero(buffer, MAX_INT_SIZE);
        sprintf(buffer, "%d", result);
        send(newsockfd, buffer, strlen(buffer), 0);
        bzero(buffer, MAX_INT_SIZE);
    }

    if (read(newsockfd, buffer, sizeof(buffer)) <= 0)
    {
        printf("Closing server...\n");
    }

    close(newsockfd);
    return 0;
}