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

db_core_funcs: ${DB_SOURCE_PATH}/db_core_funcs.c ${DB_SOURCE_PATH}/db_core_funcs.h
	$(CC) -c ${DB_SOURCE_PATH}/db_core_funcs.c -o ${OBJECT_PATH}/db_core_funcs.o

MACRO_T_DB_OPEN=-DDB_TRACE_ENABLE -DDB_ERROR_TRACE_ENABLE
t_db_open: ${TEST_PATH}/t_db_open.c
	$(CC) -Wall -g  ${TEST_PATH}/t_db_open.c -o ${BIN_PATH}/t_db_open.out ${MACRO_T_DB_OPEN}

t_db_def:	db_core_funcs ${TEST_PATH}/t_db_def.c  ${DB_SOURCE_PATH}/db_def.h
	$(CC) -Wall -g ${TEST_PATH}/t_db_def.c ${OBJECT_PATH}/db_core_funcs.o -o ${BIN_PATH}/t_db_def.out



.PHONY: clean
clean:
	${RM} -rf ${OBJECT_PATH}/*.o
	${RM} -rf ${BIN_PATH}/*