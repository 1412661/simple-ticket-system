#include <stdio.h>
#include <stdlib.h>
#include "function.h"
#include "const.h"


int main(int argc, char *argv[])
{
    char* server_ip;
    int server_port;

    if (argc != 3)
	{
        printf("[ERROR] Invalid parameters.");
        return 1;
	} else
	{
		server_port = atoi(argv[2]);
        server_ip = argv[1];
        printf("[INFO] Server IP:    %s\n", server_ip);
        printf("[INFO] Server port:  %d\n", server_port);
	}

	char* data = ask(server_ip, server_port, "hello");
    if (data != NULL)
	{
		printf("%s\n", data);
		free(data);
		return 0;
	}
	else
		return 1;
}


