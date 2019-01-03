#makefile

CC=gcc
OBJECT_FILE=db_open.o db_close.o db_create_table.o db_global.o db_core_funcs.o db_hash_function.o db_remove_table.o db_insert.o db_search.o db_update.o db_delete.o
FLAG=-Wall -g
BIN_NAME=database
MACRO_AT_BUILD=
#-DDB_TRACE_ENABLE -DDB_ERROR_TRACE_ENABLE
HEADER_FILE=db_def.h db_struct.h db_api.h db_error.h db_alloc.h db_core_funcs.h
all: $(OBJECT_FILE)

db_open.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_open.c -o $@ ${MACRO_AT_BUILD}
db_create_table.o:
	${CC} ${FLAG} -c db_create_table.c -o $@ ${MACRO_AT_BUILD}
db_close.o: 
	${CC} ${FLAG} -c db_close.c -o $@ ${MACRO_AT_BUILD}
db_global.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_global.c -o $@ ${MACRO_AT_BUILD}
db_core_funcs.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_core_funcs.c -o $@ ${MACRO_AT_BUILD}
db_hash_function.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_hash_function.c -o $@ ${MACRO_AT_BUILD}
db_remove_table.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_remove_table.c -o $@ ${MACRO_AT_BUILD}
db_insert.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_insert.c -o $@ ${MACRO_AT_BUILD}
db_search.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_search.c -o $@ ${MACRO_AT_BUILD}
db_update.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_update.c -o $@ ${MACRO_AT_BUILD}
db_delete.o: ${HEADER_FILE}
	${CC} ${FLAG} -c db_delete.c -o $@ ${MACRO_AT_BUILD}
.PHONY: clean

clean:
	rm -rf *.o ${BIN_NAME}
