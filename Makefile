CC = gcc
CFLAGS = -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

common: common.c
	$(CC) $(CFLAGS) -c common.c

clean:
	rm -f server client
