CC=g++
CFLAGS=-g -Wall -I Templates
BUILD=build

TARGET=always server client

all: $(TARGET)

server: always
	@echo "Building server..."
	$(CC) $(CFLAGS) -o $(BUILD)/server ./server/server_func_implementation.cpp ./server/server.cpp -pthread

client: always
	@echo "Building client..."
	$(CC) $(CFLAGS) -o $(BUILD)/client ./client/client.cpp

always:
	mkdir -p $(BUILD)

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD)/*

