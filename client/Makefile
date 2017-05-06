CC = gcc
LIBS =
ARGS = "-std=c99"

all:
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c ticket-client.c $(LIBS)
	$(CC) $(ARGS) -o ticket-client function.o ticket-client.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c ticket-client.c $(LIBS)
	$(CC) $(ARGS) -o ticket-client function.o ticket-client.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -c ticket-client.c $(LIBS)
	$(CC) $(ARGS) -g -o ticket-client function.o ticket-client.o $(LIBS)
	@rm *.o

cleanDebug:
	../clean.sh || :

cleanRelease:
	../clean.sh || :
