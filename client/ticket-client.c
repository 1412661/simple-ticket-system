#include <stdio.h>
#include <stdlib.h>
#include "function.h"
#include "const.h"
#include "ticket.h"


struct TicketSystem clientTicket =
{
	"TPHCM -> Ha Noi",
	{{0, 0},{0, 0},{0, 0}},
	"TPHCM -> Hue",
	{{0, 0},{0, 0},{0, 0}},
	"Ha Noi -> Da Lat",
	{{0, 0},{0, 0},{0, 0}}
};

struct TicketSystem *serverTicket;

void show(struct TicketSystem t)
{
    for (int i = 0; i < 3; i++)
	{
		printf("Route [%d]: %s\n", i+1, t.route[i].name);
		for (int j = 0; j < 3; j++)
			//if (t.route[i].ticket[j].amount > 0)
				printf("+ Ticket class: [%c]\tAmount: %d\tCost: %d$\n", 'A'+j, t.route[i].ticket[j].amount, t.route[i].ticket[j].cost);
	}
}

void menu(struct TicketSystem t, char* route, char* ticket, int* amount)
{
    printf("-----------------------------------------\n");
    printf("|    Welcome to Simple Ticket System    |\n");
    printf("-----------------------------------------\n");
    printf("[INFO] Available tickets\n");
    printf("-----------------------------------------\n");
    show(t);
    printf("-----------------------------------------\n");
    //printf("[INFO] Your tickets\n");
    //show(clientTicket);

	printf("\n");
	*route = 0;
	*ticket = 0;
	*amount = -1;
    while (*route < '1' || *route > '3' || *ticket < 'A' || *ticket > 'C' || amount < 0)
	{
		printf("Format: <Route ID> <Class> <Amount>\nExample: \"1 A 5\" will buy 5 class A ticket at route 1.\n");
		printf("Buy your ticket: ");
		scanf("%c %c %d", route, ticket, amount);

		// Small trick to prevent stack smashing attack (buffer overflow)
		char ch;
		while (((ch = getchar()) != '\n') && (ch != EOF))
		{
		}
    }
}

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

	char* response;
	char buyCommand[BUFFSIZE_VAR] = "";
	char route, ticket;
	int amount;

	printf("[INFO] Contacting with server...\n");
	serverTicket = (struct TicketSystem*)ask(server_ip, server_port, "list");
	if (serverTicket == NULL)
	{
		printf("[ERROR] Could not retrive data from server !\n");
		return 1;
	}

	menu(*serverTicket, &route, &ticket, &amount);

	printf("Your selection:\n");
	printf("+ Route  [%c] %s\n", route, serverTicket->route[route-'1'].name);
	printf("+ Class  [%c]\n", ticket);
	printf("+ Amount [%d]\n", amount);

	sprintf(buyCommand, "buy %c %c %d", route, ticket, amount);

#ifdef DEBUG
	printf("[INFO] Buy command: %s\n", buyCommand);
#endif // DEBUG

	response = ask(server_ip, server_port, buyCommand);
	if (response == NULL)
	{
		printf("[ERROR] Could not retrive data from server !\n");
	} else if (response[0] == '0')
		printf("[WARN] Could not buy ticket %c%c at amount of %d. Only %d ticket(s) left.\n", route, ticket, amount, atoi(response+1));
	else
		printf("[INFO] The ticket(s) %c%c is yours. Total cost is %d\n", route, ticket, atoi(response));

	free(response);
	free(serverTicket);

    return 0;
}


