#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define MAX_INT_SIZE 256
#define max_socket 128
int client_socket[max_socket];

int main(int argc, char *argv[])
{
	int master_socket, socket_address_len, newsockfd, activity, reading, sd, portno, max_sd;
	int opt = 1;

	struct sockaddr_in socket_address;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[MAX_INT_SIZE];

	fd_set readfds;

	for (int i = 0; i < max_socket; i++)
	{
		client_socket[i] = 0;
	}
	if (argc < 2)
	{
		fprintf(stderr, "No port.\n");
		exit(1);
	}

	// argv[0] and argv[2] used in client
	portno = atoi(argv[1]);

	// Create socket
	master_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (master_socket == 0)
	{
		perror("[ERROR] Socket Failed.\n");
		exit(0);
	}

	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("[ERROR] setting socket failed.\n");
		exit(0);
	}

	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = INADDR_ANY;
	socket_address.sin_port = htons(portno);

	if (bind(master_socket, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0)
	{
		perror("[ERROR] bind failed.\n");
		exit(0);
	}

	if (listen(master_socket, 4) < 0)
	{
		perror("[ERROR] listen failed.\n");
		exit(0);
	}
	else
	{
		printf("Listening...\n");
	}

	socket_address_len = sizeof(socket_address);
	printf("Waiting for connection .... \n");

	while (1)
	{
		FD_ZERO(&readfds);

		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		for (int i = 0; i < max_socket; i++)
		{
			sd = client_socket[i];
			if (sd > 0)
			{
				FD_SET(sd, &readfds);
			}

			if (sd > max_sd)
			{
				max_sd = sd;
			}
		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("[ERROR] select failed.\n");
		}

		newsockfd = accept(master_socket, (struct sockaddr *)&serv_addr, (socklen_t *)&socket_address_len);

		if (FD_ISSET(master_socket, &readfds))
		{
			if (newsockfd < 0)
			{
				perror("[ERROR] accept failed.\n");
				exit(0);
			}
			else
			{
				printf("Connected with client socket number: %d \n", newsockfd);
			}
			bzero(buffer, MAX_INT_SIZE);
			for (int i = 0; i < max_socket; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = newsockfd;
					break;
				}
			}
		}

		for (int i = 0; i < max_socket; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				if ((reading = read(sd, buffer, MAX_INT_SIZE)) == 0)
				{
					getpeername(sd, (struct sockaddr *)&socket_address, (socklen_t *)&socket_address_len);
					printf("Client socket %d disconnected\n", ntohs(socket_address.sin_port));
					close(sd);
					client_socket[i] = 0;
				}

				else
				{
					printf("Client socket %d sent message : %s\n", ntohs(socket_address.sin_port), buffer);
					int num1 = 0;
					int num2 = 0;
					int j = 0;
					int flag = 0;
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

					int result = 0;
					if (buffer[j] == '+')
					{
						result += num1 + num2;
					}
					else if (buffer[j] == '-')
					{
						result += num1 - num2;
					}
					else if (buffer[j] == '/')
					{
						result += num1 / num2;
					}
					else
					{
						result += num1 * num2;
					}

					printf("Sending reply : %i\n", result);
					bzero(buffer, MAX_INT_SIZE);
					sprintf(buffer, "%d", result);
					send(sd, buffer, strlen(buffer), 0);
					bzero(buffer, MAX_INT_SIZE);
				}
			}
		}
	}

	return 0;
}