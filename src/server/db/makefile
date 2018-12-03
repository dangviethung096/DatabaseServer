#makefile

CC=gcc
OBJECT_FILE=db_open.o db_close.o db_create_table.o db_global.o db_core_funcs.o db_hash_function.o
FLAG=-Wall -g
BIN_NAME=database
MACRO_AT_BUILD=-DDB_TRACE_ENABLE -DDB_ERROR_TRACE_ENABLE
HEADER_FILE=db_def.h db_struct.h db_api.h db_error.h db_alloc.h db_core_funcs.h
all: $(OBJECT_FILE)

db_open.o: ${HEADER_FILE}
	${CC} -c db_open.c -o $@ ${MACRO_AT_BUILD}
db_create_table.o:
	${CC} -c db_create_table.c -o $@ ${MACRO_AT_BUILD}
db_close.o: 
	${CC} -c db_close.c -o $@ ${MACRO_AT_BUILD}
db_global.o: ${HEADER_FILE}
	${CC} -c db_global.c -o $@ ${MACRO_AT_BUILD}
db_core_funcs.o: ${HEADER_FILE}
	${CC} -c db_core_funcs.c -o $@ ${MACRO_AT_BUILD}
db_hash_function.o: ${HEADER_FILE}
	${CC} -c db_hash_function.c -o $@ ${MACRO_AT_BUILD}

.PHONY: clean

clean:
	rm -rf *.o ${BIN_NAME}