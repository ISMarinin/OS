#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Mess.h"
#include "zmq.h"

void help () {
	printf("\tcreate id [parent] -- создать узел\n");
	printf("\tremove id -- удалить узел\n");
	printf("\texec   id [param] -- выполнить узел\n");
	printf("\tping [id] -- проверка узла на доступность\n");
	printf("\texit   -- выход\n");
}

int main () {
	printf("Client Starting…\n");

	void* context = zmq_ctx_new();  //создаем новый контекст
	if (!context) {
		fprintf(stderr, "zmq_ctx_new error\n");
		exit(-1);
	}
	void* request = zmq_socket(context, ZMQ_PAIR); // создаем сокет в указанном контексте и возвращаем дескриптор во вновь созданный сокет
	if (!request) {
		fprintf(stderr, "zmq_socket error\n");
		exit(-2);
	}
	zmq_connect(request, "tcp://localhost:4040"); // соединяем сокет с конечной точкой
	
	help();

	long long timer = 0;
	ERROR_TYPE result;
	int pid, ping;
	while (1) {
		int id = 0, parent = 0;
		char command[100] = {'\0'}, param[100] = {'\0'};
		Message mess;
		scanf("%s",command);
		mess.command = CreateCommand(command);
		switch (mess.command) {
			case CREATE:
				 scanf("%d %d", &id, &parent);
				 break;
			case REMOVE:
				 scanf("%d", &id);
				 break;
			case EXEC:
				 scanf("%d %s", &id, param);
				 break;
			case PING:
				 scanf("%d", &id);
				 break;
			case EXIT:
				 break;
			default:
				fgets(command, 100, stdin);
				break;
		}
		mess.param = CreateParameter(param);
		mess.id = id;
		mess.parent = parent;

		zmq_send(request, (void*)&mess, sizeof(Message), 0); // ставим в очередь сообщение, созданное из буфера, на который ссылаются аргументы mess и sizeof

		if (mess.command == EXIT) {
			break;
		}

		zmq_recv(request, &result, sizeof(ERROR_TYPE), 0); // сообщение из request сохраняем в result

		if (result == SUCCESS) {
			printf("OK");
			switch (mess.command) {
				case CREATE:
					zmq_recv(request, &pid, sizeof(int),0);
					printf(": %d", pid);
					break;
				case EXEC:
					printf(":%d", mess.id);
					if (mess.param == TIME) {
						zmq_recv(request, &timer, sizeof(long long),0);
						printf(":%lld.%lld", timer / 1000, timer % 1000);
					}
					break;
				case PING:
					zmq_recv(request, &ping, sizeof(int),0);
					printf(" %d", ping);
					break;
				default:
					break;
			}
			printf("\n");
		} 
		else {
			printf("ERROR:%d: ", mess.id);
			switch (result) {
				case ALREADY_EXIST:
					printf("node already exist\n");
					break;
				case PARENT_NOT_FOUND:
					printf("parent not found\n");
					break;
				case PARENT_IS_UNAVAILABLE:
					printf("parent is unavailable\n");
					break;
				case NODE_NOT_FOUND:
					printf("node not found\n");
					break;
				case NODE_IS_UNAVAILABLE:
					printf("node is unavailable\n");
					break;
				case READ_ERROR:
					printf("can't read from node\n");
					break;
				default:
					break;
			}
		}
	}
	zmq_close(request); // уничтожаем сокет, на который ссылается request
	zmq_ctx_destroy(context); // уничтожаем context
	return 0;
}