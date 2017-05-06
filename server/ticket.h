#ifndef _TICKET_H_
#define _TICKET_H_

/**
 * @file ticket.h
 * @brief Define data structrucre
 */


#include <stdint.h>

struct Ticket
{
    int amount;
    int cost;
};

struct Route
{
    char name[100];
    struct Ticket ticket[3];
};

struct TicketSystem
{
    struct Route route[3];
};

#endif // _TICKET_H_
