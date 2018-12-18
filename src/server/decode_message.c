#include "lib/server_struct.h"
#include "lib/server_def.h"

void decode_msg_search(U8bit * search)
{
    /* Parse message for search in db */
}

void decode_msg_delete(U8bit * delete)
{
    /* Parse message for delete in db */
}

void decode_msg_update(U8bit * buffer)
{
    /* Parse message for update in db */
}

void decode_msg_insert(U8bit * buffer)
{
    /* Parse message for insert in db */
}

void decode_message(U8bit * buffer)
{
    /* Parse message for db */
    command_code_t code = *((command_code_t *) buffer);
    SERVER_TRACE(("SERVER:decode_message: code = %d\n", (int) code));
    switch(code)
    {
        case SEARCH_CODE:
            decode_msg_search(buffer + COMMAND_CODE_SIZE);
            break; 
        case INSERT_CODE:
            decode_msg_insert(buffer + COMMAND_CODE_SIZE);
            break;
        case UPDATE_CODE:
            decode_msg_update(buffer + COMMAND_CODE_SIZE);
            break;
        case DELETE_CODE:
            decode_msg_delete(buffer + COMMAND_CODE_SIZE);
            break;
        default:
            SERVER_TRACE(("SERVER:decode_message:ERROR: cannot detect code\n"));
    }
}