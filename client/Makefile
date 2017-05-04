CC = gcc
LIBS =
ARGS = "-std=c99"

all:
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c HTTPClient.c $(LIBS)
	$(CC) $(ARGS) -c linklist.c $(LIBS)
	$(CC) $(ARGS) -o 1412661 function.o HTTPClient.o linklist.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c HTTPClient.c $(LIBS)
	$(CC) $(ARGS) -c linklist.c $(LIBS)
	$(CC) $(ARGS) -o 1412661 function.o HTTPClient.o linklist.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -c HTTPClient.c $(LIBS)
	$(CC) $(ARGS) -g -c linklist.c $(LIBS)
	$(CC) $(ARGS) -g -o 1412661 function.o HTTPClient.o linklist.o $(LIBS)
	@rm *.o

clean:
	@rm *.save 2> /dev/null
	@rm *.pdf 2> /dev/null
	@rm *.html 2> /dev/null
	@rm *.o 2> /dev/null
