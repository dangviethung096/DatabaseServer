#make file

######################################################
#SECTION 1: Define VARIABLE
######################################################
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
FLAGS="-Wall -g"
MACRO_AT_BUILD="-DDB_TRACE_ENABLE -DDB_ERROR_TRACE_ENABLE"
OBJECT_FILE_DB=

#####################################################
#SECTION 2: FUNCTIONS
#####################################################

BUILD_DB()
{
	NOW_PATH=${PWD}
	cd ${DB_SOURCE_PATH};
	make all
	mv ${DB_SOURCE_PATH}/*.o ${OBJECT_PATH}/
	OBJECT_FILE_DB=$(find ${OBJECT_PATH} -name "db_*.o")
	#return old path
	cd ${NOW_PATH}
}



BUILD_T_DB_CREATE_TABLE()
{
	echo "${CC} -c ${TEST_PATH}/t_db_create_table.c -o ${OBJECT_PATH}/t_db_create_table.o"
	${CC} -c ${TEST_PATH}/t_db_create_table.c -o ${OBJECT_PATH}/t_db_create_table.o

	echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_create_table.o -o ${BIN_PATH}/t_db_create_table.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_create_table.o -o ${BIN_PATH}/t_db_create_table.out
}


BUILD_T_DB_OPEN()
{
	${CC} ${FLAGS} ${TEST_PATH}/t_db_open.c -o ${BIN_PATH}/t_db_open.out ${MACRO_T_DB_OPEN}
}

BUILD_TEST()
{
	BUILD_DB
	BUILD_T_DB_OPEN
	BUILD_T_DB_CREATE_TABLE
}
#####################################################
#SECTION 3: RULES
#####################################################
# server: ${LIB_PATH}/constants.h ${SERVER_PATH}/server.c
# 	$(CC) -Wall -g $(SERVER_PATH)/server.c -o ${BIN_PATH}/server -lpthread
# client: ${LIB_PATH}/constants.h ${CLIENT_PATH}/client.c
# 	$(CC) -Wall -g ${CLIENT_PATH}/client.c -o ${BIN_PATH}/client

# db_core_funcs: ${DB_SOURCE_PATH}/db_core_funcs.c ${DB_SOURCE_PATH}/db_core_funcs.h
# 	$(CC) -c ${DB_SOURCE_PATH}/db_core_funcs.c -o ${OBJECT_PATH}/db_core_funcs.o



# MACRO_T_DB_OPEN=-DDB_TRACE_ENABLE -DDB_ERROR_TRACE_ENABLE
# OBJECT_FILE_DB=${OBJECT_PATH}/db_open.o ${OBJECT_PATH}/db_create_table.o ${OBJECT_PATH}/db_close.o
# DB_FILE_IN_SRC_DIR=${find ${DB_SOURCE_PATH} -name "*.c" && find ${DB_SOURCE_PATH} -name "*.h"}


# t_db_open: ${TEST_PATH}/t_db_open.c
# 	${CC} -Wall -g  ${TEST_PATH}/t_db_open.c -o ${BIN_PATH}/t_db_open.out ${MACRO_T_DB_OPEN}

# t_db_def:	db_core_funcs ${TEST_PATH}/t_db_def.c  ${DB_SOURCE_PATH}/db_def.h
# 	${CC} -Wall -g ${TEST_PATH}/t_db_def.c ${OBJECT_PATH}/db_core_funcs.o -o ${BIN_PATH}/t_db_def.out

# t_db_create_table: ${TEST_PATH}/t_db_create_table.c ${DB_FILE_IN_SRC_DIR}
# 	BUILD_DB
# 	mv ${OBJECT_PATH}/*.o ${TEST_PATH}/
# 	${CC} -c ${TEST_PATH}/t_db_create_table.c -o ${TEST_PATH}/t_db_create_table.o
# 	${CC} -Wall -g ${OBJECT_FILE_DB} ${TEST_PATH}/t_db_create_table.o


# .PHONY: clean
# clean:
# 	${RM} -rf ${OBJECT_PATH}/*.o
# 	${RM} -rf ${BIN_PATH}/*

#########################################################################
#clean object file
${RM} -rf ${OBJECT_PATH}/*.o
${RM} -rf ${BIN_PATH}/*
BUILD_TEST