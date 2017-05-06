CC = gcc
LIBS = -lpthread

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# Can use #pragma GCC diagnostic ignored in code
# #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
# #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

IGNORE = -Wno-incompatible-pointer-types -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
ARGS = -std=c99

all:
	$(CC) $(IGNORE) $(ARGS) -c ticket-server.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -c function.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -o ticket-server ticket-server.o function.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(IGNORE) $(ARGS) -c ticket-server.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -c function.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -o ticket-server ticket-server.o function.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c ticket-server.c $(LIBS)
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -o ticket-server ticket-server.o function.o $(LIBS)
	@rm *.o

cleanDebug:
	../clean.sh || :

cleanRelease:
	../clean.sh || :
