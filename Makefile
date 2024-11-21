CC = gcc
CFLAGS = -Wall -pthread

SRC = src/main.c src/irc_client.c src/network.c src/utils.c
OBJ = $(SRC:.c=.o)

TARGET = build/irc_client

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -rf $(OBJ) $(TARGET)