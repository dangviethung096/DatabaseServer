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
MACRO_SERVER="-DSERVER_ENABLE_TRACE"
OBJECT_FILE_DB=
#####################################################
#SECTION 2: FUNCTIONS
#####################################################

BUILD_DB()
{
	printf "\n\n=============================================================================\n"
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
	# printf "\n\n=============================================================================\n"
	# echo "${CC} -c ${TEST_PATH}/t_db_create_table.c -o ${OBJECT_PATH}/t_db_create_table.o"
	${CC} -c ${TEST_PATH}/t_db_create_table.c -o ${OBJECT_PATH}/t_db_create_table.o
	# printf "\n\n=============================================================================\n"
	# echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_create_table.o -o ${BIN_PATH}/t_db_create_table.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_create_table.o -o ${BIN_PATH}/t_db_create_table.out
}


BUILD_T_DB_OPEN()
{
	# printf "\n\n=============================================================================\n"
	# echo "${CC} -c ${TEST_PATH}/t_db_open.c -o ${OBJECT_PATH}/t_db_open.o"
	${CC} -c ${TEST_PATH}/t_db_open.c -o ${OBJECT_PATH}/t_db_open.o
	# printf "\n\n=============================================================================\n"
	# echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_open.o -o ${BIN_PATH}/t_db_open.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_open.o -o ${BIN_PATH}/t_db_open.out
}

BUILD_T_DB_DEF()
{
	# printf "\n\n=============================================================================\n"
	# echo "${CC} -c ${TEST_PATH}/t_db_def.c -o ${OBJECT_PATH}/t_db_def.o"
	${CC} -c ${TEST_PATH}/t_db_def.c -o ${OBJECT_PATH}/t_db_def.o
	# printf "\n\n=============================================================================\n"
	# echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_def.o -o ${BIN_PATH}/t_db_def.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_def.o -o ${BIN_PATH}/t_db_def.out
}


BUILD_T_DB_CREATE_DB()
{
	# printf "\n\n=============================================================================\n"
	# echo "${CC} -c ${TEST_PATH}/t_db_read_db.c -o ${OBJECT_PATH}/t_db_read_db.o"
	${CC} -c ${TEST_PATH}/t_db_create_db.c -o ${OBJECT_PATH}/t_db_create_db.o
	# printf "\n\n=============================================================================\n"
	# echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_read_db.o -o ${BIN_PATH}/t_db_read_db.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_create_db.o -o ${BIN_PATH}/t_db_create_db.out
}

BUILD_T_DB_REMOVE_TABLE()
{
	# printf "\n\n=============================================================================\n"
	# echo "${CC} -c ${TEST_PATH}/t_db_remove_table.c -o ${OBJECT_PATH}/t_db_remove_table.o"
	${CC} -c ${TEST_PATH}/t_db_remove_table.c -o ${OBJECT_PATH}/t_db_remove_table.o
	# printf "\n\n=============================================================================\n"
	# echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_remove_table.o -o ${BIN_PATH}/t_db_remove_table.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_remove_table.o -o ${BIN_PATH}/t_db_remove_table.out
}

BUILD_T_DB_INSERT()
{
	#printf "\n\n=============================================================================\n"
	#echo "${CC} -c ${TEST_PATH}/t_db_insert.c -o ${OBJECT_PATH}/t_db_insert.o"
	${CC} -c ${TEST_PATH}/t_db_insert.c -o ${OBJECT_PATH}/t_db_insert.o
	#printf "\n\n=============================================================================\n"
	#echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_insert.o -o ${BIN_PATH}/t_db_insert.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_insert.o -o ${BIN_PATH}/t_db_insert.out
}

BUILD_T_DB_COMMON_TEST()
{
	#printf "\n\n=============================================================================\n"
	#echo "${CC} -c ${TEST_PATH}/t_db_common_test.c -o ${OBJECT_PATH}/t_db_common_test.o"
	${CC} -c ${TEST_PATH}/t_db_common_test.c -o ${OBJECT_PATH}/t_db_common_test.o
	#printf "\n\n=============================================================================\n"
	#echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_common_test.o -o ${BIN_PATH}/t_db_common_test.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_common_test.o -o ${BIN_PATH}/t_db_common_test.out
}

BUILD_T_DB_SEARCH()
{
	#printf "\n\n=============================================================================\n"
	#echo "${CC} -c ${TEST_PATH}/t_db_search.c -o ${OBJECT_PATH}/t_db_search.o"
	${CC} -c ${TEST_PATH}/t_db_search.c -o ${OBJECT_PATH}/t_db_search.o
	#printf "\n\n=============================================================================\n"
	#echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_search.o -o ${BIN_PATH}/t_db_search.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_search.o -o ${BIN_PATH}/t_db_search.out
}

BUILD_T_DB_DELETE()
{
	#printf "\n\n=============================================================================\n"
	#echo "${CC} -c ${TEST_PATH}/t_db_delete.c -o ${OBJECT_PATH}/t_db_delete.o"
	${CC} -c ${TEST_PATH}/t_db_delete.c -o ${OBJECT_PATH}/t_db_delete.o
	#printf "\n\n=============================================================================\n"
	#echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_delete.o -o ${BIN_PATH}/t_db_delete.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_delete.o -o ${BIN_PATH}/t_db_delete.out
}

BUILD_T_DB_UPDATE()
{
	#printf "\n\n=============================================================================\n"
	#echo "${CC} -c ${TEST_PATH}/t_db_update.c -o ${OBJECT_PATH}/t_db_update.o"
	${CC} -c ${TEST_PATH}/t_db_update.c -o ${OBJECT_PATH}/t_db_update.o
	#printf "\n\n=============================================================================\n"
	#echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_update.o -o ${BIN_PATH}/t_db_update.out"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/t_db_update.o -o ${BIN_PATH}/t_db_update.out
}


BUILD_SERVER()
{
	printf "\n\n=============================================================================\n"
	echo "${CC} -c ${SERVER_PATH}/server.c -o ${OBJECT_PATH}/server.o -lpthread"
	${CC} -c ${SERVER_PATH}/server.c -o ${OBJECT_PATH}/server.o -lpthread
	echo "${CC} -c ${SERVER_PATH}/server_global.c -o ${OBJECT_PATH}/server_global.o"
	${CC} -c ${SERVER_PATH}/server_global.c -o ${OBJECT_PATH}/server_global.o
	echo "${CC} -c ${SERVER_PATH}/process_message.c -o ${OBJECT_PATH}/process_message.o ${MACRO_SERVER}"
	${CC} -c ${SERVER_PATH}/process_message.c -o ${OBJECT_PATH}/process_message.o ${MACRO_SERVER}
	
	printf "\n\n=============================================================================\n"
	echo "${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/server.o ${OBJECT_PATH}/server_global.o ${OBJECT_PATH}/process_message.o -o ${BIN_PATH}/server.out -lpthread ${MACRO_SERVER}"
	${CC} ${FLAGS} ${OBJECT_FILE_DB} ${OBJECT_PATH}/server.o ${OBJECT_PATH}/server_global.o ${OBJECT_PATH}/process_message.o -o ${BIN_PATH}/server.out -lpthread ${MACRO_SERVER}
}

BUILD_GEN_DATA()
{
	printf "\n\n=============================================================================\n"
	echo "${CC} ${FLAGS} ${TEST_PATH}/t_db_gen_data.c -o ${BIN_PATH}/t_db_gen_data.out"
	${CC} ${FLAGS} ${TEST_PATH}/t_db_gen_data.c -o ${BIN_PATH}/t_db_gen_data.out
}

BUILD_TEST()
{
	printf "\nBUILD_DB"
	BUILD_DB
	printf "\nBUILD_T_DB_OPEN"
	BUILD_T_DB_OPEN
	printf "\nBUILD_T_DB_CREATE_TABLE"
	BUILD_T_DB_CREATE_TABLE
	printf "\nBUILD_T_DB_DEF"
	BUILD_T_DB_DEF
	printf "\nBUILD_T_DB_CREATE_TABLE"
	BUILD_T_DB_CREATE_DB
	printf "\nBUILD_T_DB_REMOVE_TABLE"
	BUILD_T_DB_REMOVE_TABLE
	printf "\nBUILD_T_DB_INSERT"
	BUILD_T_DB_INSERT
	printf "\nBUILD_T_DB_COMMON_TEST"
	BUILD_T_DB_COMMON_TEST
	printf "\nBUILD_T_DB_SEARCH"
	BUILD_T_DB_SEARCH
	printf "\nBUILD_T_DB_DELETE"
	BUILD_T_DB_DELETE
	printf "\nBUILD_T_DB_UPDATE"
	BUILD_T_DB_UPDATE
	printf "\nBUILD_SERVER"
	BUILD_SERVER
	printf "\nBUILD_GEN_DATA"
	BUILD_GEN_DATA
	printf "\n"
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