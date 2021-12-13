#ifndef MESS_H
#define MESS_H

#include <string.h>

typedef enum ERROR_TYPE {
    SUCCESS = 0,

    ALREADY_EXIST,
    PARENT_NOT_FOUND,
    PARENT_IS_UNAVAILABLE,

    NODE_NOT_FOUND,
    NODE_IS_UNAVAILABLE,
    READ_ERROR,
} ERROR_TYPE;

typedef enum COMM_TYPE {
    CREATE = 0,
    REMOVE,
    EXEC,
    PING,
    EXIT,
    UNKNOWN_COMM
} COMM_TYPE;

typedef enum PARAM_TYPE {
    START = 0,
    STOP,
    TIME,
    CLOSE,
    UNKNOWN_PARAM
} PARAM_TYPE;

typedef struct Message {
    COMM_TYPE command;
    PARAM_TYPE param;
	int id, parent;
} Message;

COMM_TYPE CreateCommand (char* command);
PARAM_TYPE CreateParameter (char* param);

#endif