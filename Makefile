CC = gcc
SERVER_SRCS = src/server.c src/cli_addrs.c
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
SERVER_TARGET = server.exe
CLIENT_SRCS = src/client.c
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
CLIENT_TARGET = client.exe
SHARED_SRCS = src/sock_utils.c 
SHARED_OBJS = $(SHARED_SRCS:.c=.o)
FLAGS = -std=c23 -Wall -Werror -Wconversion
INCLUDE = 
LIB = -lws2_32

all: server client

server: $(SERVER_OBJS) $(SHARED_OBJS)
	$(CC) $^ $(FLAGS) $(LIB) -o $(SERVER_TARGET)

client: $(CLIENT_OBJS) $(SHARED_OBJS)
	$(CC) $^ $(FLAGS) $(LIB) -o $(CLIENT_TARGET)

%.o: %.c
	$(CC) -c $< $(FLAGS) $(INCLUDE) -o $@

clean:
	del /q src\*.o
	del /q server.exe client.exe
