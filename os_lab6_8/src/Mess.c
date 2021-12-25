#include "Mess.h"

COMM_TYPE CreateCommand (char* command) {
	if (strcmp(command, "create") == 0) {
		return CREATE;
	} else if (strcmp(command, "remove") == 0) {
		return REMOVE;
	} else if (strcmp(command, "exec") == 0) {
		return EXEC;
	} else if (strcmp(command, "ping") == 0) {
		return PING;
	} else if (strcmp(command, "exit") == 0) {
		return EXIT;
	} else {
        return UNKNOWN_COMM;
    }
}

PARAM_TYPE CreateParameter (char* param) {
    if (strcmp(param, "start") == 0) {
		return START;
	} else if (strcmp(param, "stop") == 0) {
		return STOP;
	} else if (strcmp(param, "time") == 0) {
		return TIME;
	} else {
        return UNKNOWN_PARAM;
    }
}