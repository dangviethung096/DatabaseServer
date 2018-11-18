#make file
ROOT_PATH=${PWD}
BIN_PATH=${ROOT_PATH}/bin
SOURCE_PATH=${PWD}/src
CLIENT_PATH=${SOURCE_PATH}/client
SERVER_PATH=${SOURCE_PATH}/server
LIB_PATH=${SOURCE_PATH}/lib
TEST_PATH=${SOURCE_PATH}/test
OBJECT_PATH=${ROOT_PATH}/object_file
DB_SOURCE_PATH=${SOURCE_PATH}/server/db
RM=rm

CC=gcc


server: ${LIB_PATH}/constants.h ${SERVER_PATH}/server.c
	$(CC) -Wall -g $(SERVER_PATH)/server.c -o ${BIN_PATH}/server -lpthread
client: ${LIB_PATH}/constants.h ${CLIENT_PATH}/client.c
	$(CC) -Wall -g ${CLIENT_PATH}/client.c -o ${BIN_PATH}/client

${OBJECT_PATH}/db_alloc.o: ${DB_SOURCE_PATH}/db_alloc.c ${DB_SOURCE_PATH}/db_alloc.h
	$(CC) -c ${DB_SOURCE_PATH}/db_alloc.c -o ${OBJECT_PATH}/db_alloc.o

MACRO_T_DB_OPEN=-DDB_TRACE_ENABLE
t_db_open: ${TEST_PATH}/t_db_open.c ${OBJECT_PATH}/db_alloc.o
	$(CC) -Wall -g  ${TEST_PATH}/t_db_open.c ${OBJECT_PATH}/db_alloc.o -o ${BIN_PATH}/t_db_open.out ${MACRO_T_DB_OPEN}



.PHONY: clean
clean:
	${RM} -rf ${OBJECT_PATH}/*.o
	${RM} -rf ${BIN_PATH}/*