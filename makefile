#make file
ROOT_PATH=${PWD}
CLIENT_PATH=${ROOT_PATH}/src/client
SERVER_PATH=${ROOT_PATH}/src/server
LIB_PATH=${ROOT_PATH}/src/lib
BIN_PATH=${ROOT_PATH}/bin

CC=gcc

server: ${LIB_PATH}/constants.h ${SERVER_PATH}/server.c
	$(CC) -g $(SERVER_PATH)/server.c -o ${BIN_PATH}/server -lpthread
client: ${LIB_PATH}/constants.h ${CLIENT_PATH}/client.c
	$(CC) -g ${CLIENT_PATH}/client.c -o ${BIN_PATH}/client