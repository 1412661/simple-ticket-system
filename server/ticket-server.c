/*

This is a very simple HTTP server. Default port is 9999
To start a server at port 50000:
$ ./webserver -p 50000

http://stackoverflow.com/questions/9681531/graceful-shutdown-server-socket-in-linux
- prevent accept() from adding more clientfd
- have a list of the open sockets somewhere and to wait until they are all properly closed which means:
	+ using shutdown() to tell the client that you will no longer work on that socket
	+ call read() for a while to make sure that all the client has sent in the meantime has been pulled
	+ then using close() to free each client socket.
- THEN, you can safely close() the listening socket.

*/

// Thread & mutex
// http://www.thegeekstuff.com/2012/05/c-mutex-examples/?refcom
// http://softpixel.com/~cwright/programming/threads/threads.c.php

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include <pthread.h>
#include <semaphore.h>

#include "const.h"
#include "type.h"
#include "function.h"
#include "ticket.h"

// Start socket server
void startServer(int port);

// Child thread that will reponse HTTP request
void respond(void*);

int socketfd;				// Socket File Descript that define a socket
struct ThreadList thread;			// Threads manager
pthread_mutex_t mutex;		// Mutex to lock thread manager

int keepRunning = 1;
//static volatile int keepRunning = 1;


struct TicketSystem db =
{
	"TPHCM - Ha Noi",
	{{50, 100},{80, 80},{40, 60}},
	"TPHCM - Hue",
	{{35, 60},{54, 50},{48, 40}},
	"Ha Noi - Da Lat",
	{{70, 120},{50, 90},{30, 70}}
};


void intHandler()
{
    printf("\nCtrl + C catched !\n");
    printf("[INFO] Waiting for all sockets to be closed...\n");
    printf("[INFO] Thread left: %d\n", thread.count);

    // Stop accept more connection
    keepRunning = 0;

    // Wait for all thread exit	// will very soon
    while (thread.count != 0)
    {
    }

    close(socketfd);	// accept() will return -1
    //Prevent from accepting any more connection

    printf("[WARNING] Socket server offline\n");
}

int main(int argc, char* argv[])
{
    // Warning about current working folder
    printf("-----------------------------------------\n");
    printf("|    Welcome to Simple Ticket System    |\n");
    printf("-----------------------------------------\n");
    printf("[INFO] Max connection: %d\n", CONNMAX);
    printf("[INFO] Last build: 07:05 am 06.05.2017.\n");

    // Put a trap for SIGINT (Ctrl+C)
    signal(SIGINT, intHandler);

    // Thread management task
    thread.count = 0;
    memset(thread.status, 0, CONNMAX*sizeof(thread.status[0]));
    pthread_mutex_init(&mutex, NULL);

    // Set default port or user-defined port
    int port = DEFAULT_PORT;
    if (argc == 2)
        port = atoi(argv[1]);

	// Start socket server
    startServer(port);

    while (keepRunning)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
#ifdef DEBUG_MODE
        printf("[INFO] Waiting for new connection...\n");
#endif // DEBUG_MODE
        int clientfd = accept(socketfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clientfd >= 0)
        {
            printf("[INFO] Number of thread: %d\n", thread.count);
            if (thread.count == CONNMAX)
            {
                // Connection is close by these steps:
                // 1. Receive remain data from client
                // 2. Shutdown the socket
                // 3. Close the socket
                char* tmp = (char*)malloc(BUFFSIZE_DATA);
                recv(clientfd, tmp, BUFFSIZE_DATA, 0);
                free(tmp);
                shutdown(clientfd, SHUT_RDWR);
                close(clientfd);
            }
            else
            {
#ifdef DEBUG_MODE
                printf("[INFO] Connection with descriptor %d is accepted\n", clientfd);
#endif // DEBUG_MODE
                pthread_mutex_lock(&mutex);
                int freeThreadID = findEmptyThread(thread.status);
                thread.count++;
                thread.status[freeThreadID] = 1;
                pthread_mutex_unlock(&mutex);

#ifdef DEBUG_MODE
                printf("[INFO] Thread #%d/%d is used\n", freeThreadID+1, CONNMAX);
#endif // DEBUG_MODE

                struct DataToPassToThread data;
                data.socketfd = clientfd;
                data.thread_id = freeThreadID;
                pthread_create(&(thread.handler[freeThreadID]), NULL, respond, &data);
                pthread_detach(thread.handler[freeThreadID]);
            }
        }
    }

    pthread_exit(NULL);
}

void startServer(int port)
{

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        printf("[ERROR] socket(): could not initialize socket");

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // When port is used by another program or not release by OS,
    // switch to another port (port--)
    while (1)
    {
        serv_addr.sin_port = htons(port);
        if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            printf("[ERROR] bind(): could not bind socket at port %d\n", port--);
        else
            break;
    }

    if (listen(socketfd, BUFFSIZE_DATA) != 0)
        printf("[ERROR] listen(): could not listen socket");

    printf("[INFO] Socket server started at port %s%d%s width pid %s%d%s\n", "\033[92m", port, "\033[0m", "\033[92m", getpid(), "\033[0m");
}

//client connection
void respond(void* arg)
{
    struct DataToPassToThread* data = (struct DataToPassToThread*)arg;

    char mesg[BUFFSIZE_DATA] = "";		// set all byte to zero
    int rcvd = recv(data->socketfd, mesg, BUFFSIZE_DATA, 0);
    if (rcvd < 0)
    {
#ifdef DEBUG_MODE
        printf("[ERROR] recv() error\n");
#endif // DEBUG_MODE
    }
    else if (rcvd == 0)
    {
#ifdef DEBUG_MODE
        printf("[ERROR] Client disconnected upexpectedly.\n");
#endif // DEBUG_MODE
    }
    else
    {
        printf("-----------------------------------\n");
        printf("[INFO] Client request: %s\n", mesg);

        if (strcmp(mesg, "list") == 0)
		{
			write(data->socketfd, (char*)&db, sizeof(struct TicketSystem));
		} else
		{
			char route;
			char ticket;
			int cost;
			int amount;
			char buyCommand[4];
			char response[BUFFSIZE_VAR];

            sscanf(mesg, "%s %c %c %d", buyCommand, &route, &ticket, &amount);
			if ((strcmp("buy", buyCommand) == 0) && route >= '1' && route <= '3' && ticket >= 'A' && ticket <= 'C' && amount > 0)
			{
				pthread_mutex_lock(&mutex);
				if (db.route[route-'1'].ticket[ticket-'A'].amount >= amount)
				{
					db.route[route-'1'].ticket[ticket-'A'].amount -= amount;
					cost = db.route[route-'1'].ticket[ticket-'A'].cost * amount;

                    sprintf(response, "%d", cost);

					write(data->socketfd, response, strlen(response));
				} else
				{
					response[0] = '0';
					sprintf(response+1, "%d", db.route[route-'1'].ticket[ticket-'A'].amount);
					write(data->socketfd, response, strlen(response));
				}

				printf("[INFO] Server response: %s\n", response);
				pthread_mutex_unlock(&mutex);
			} else
			{
                printf("[WARN] Invalid buy command from client: %s\n", mesg);
			}
		}
    }

    // Shutdown send/receive function on the socket
    shutdown(data->socketfd, SHUT_RDWR);

    // Close and free the socket
    close(data->socketfd);

#ifdef DEBUG_MODE
    printf("[INFO] Connection at pid %d is closed\n", getpid());
#endif // DEBUG_MODE

	// Update that a thread is exited
	pthread_mutex_lock(&mutex);
	thread.count--;
	thread.status[data->thread_id] = 0;
	pthread_mutex_unlock(&mutex);
}
