#make file
ROOT_PATH=${PWD}
CLIENT_PATH=${ROOT_PATH}/client
SERVER_PATH=${ROOT_PATH}/server
LIB_PATH=${ROOT_PATH}/lib
BIN_PATH=${ROOT_PATH}/../bin

CC=gcc

server: ${LIB_PATH}/constants.h ${SERVER_PATH}/server.c
	$(CC) -g $(SERVER_PATH)/server.c -o ${BIN_PATH}/server -lpthread
client: ${LIB_PATH}/constants.h ${CLIENT_PATH}/client.c
	$(CC) -g ${CLIENT_PATH}/client.c -o ${BIN_PATH}/client