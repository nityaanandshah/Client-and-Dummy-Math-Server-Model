# Client and Dummy Math Server Model

Implementation of a simple client-server model where a client program can chat with a dummy math server. The communication between the client and server goes as follows:

1. The server gets started first and listens to a known port.
2. The client program is started with the server IP and port provided in the command line.
3. The client connects to the server and asks for user input. The user enters a simple arithmetic expression like "5+5" or "6-2". The input is sent to the server through the connected socket.
4. The server receives the input from the client socket, evaluates the expression, and sends the result back to the client.
5. The client displays the result from the server to the user and prompts for the next input until the user terminates the client program by Ctrl+C.

## Server1

Instructions are as follows:
- Run the file named 'server1.c'.
- The file requires two inputs: the file name ('server1.c') and the port number (5555).
- The server file will then execute as mentioned in the lab instructions.

## Server2

Instructions are as follows:
- Run the file named 'server2.c'.
- The file requires two inputs: the file name ('server2.c') and the port number (5555).
- The server file will then execute as mentioned in the lab instructions.

## Server3

Instructions are as follows:
- Run the file named 'server3.c'.
- The file requires two inputs: the file name ('server3.c') and the port number (5555).
- The server file will then execute as mentioned in the lab instructions.

## Client

Instructions are as follows:
- Run the file named 'client.c'.
- The file requires two inputs: the file name ('client.c') and the IP address (127.0.0.x 5555).
- The client file will then execute as mentioned in the lab instructions.

## Comparative Performance

The author's view, based on their individual understanding, regarding the performance of the three servers (with given specifications) are as follows:
(NOTE: The I/O of the code files provided by the author is the same as mentioned in the lab instructions.)

- Server1 can handle only one client request at a time and hence is inefficient in practical implementation.
- Server2 can handle one or more client requests at a time (simultaneously), but considering it requires forking a child process each time a new client request is served to the server, it has the following disadvantages:
  - Threads are difficult to code, debug, and sometimes have undesirable results.
  - The overhead associated with switching of context is high and undesirable.
  - It is not practical and efficient for a large number of clients (possibly hundreds).
  - There is a possibility of deadlocks.
- Server3 can handle one or more client requests at a time (simultaneously) and uses the 'select' system call. The advantages of this approach are listed below:
  - It is like an 'interrupt handler' and is activated as soon as any file descriptor sends over data.
  - It monitors multiple file descriptors.

Hence, based on the above observations, Server3 is the most efficient server.
