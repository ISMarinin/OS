#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include "zmq.h"
#include "Mess.h"
#include "Tree.h"

Tree* tree = NULL;

void termination (int code) {
	if (tree) {
		Destroy(tree);
	}
	exit(code);
}

typedef struct Pipes {
	int pipe1[2];
    int pipe2[2];
} Pipes;

long long CurrentTime() {
   struct timeval te;
   if (gettimeofday(&te, NULL) != 0) {
      fprintf(stderr, "time error\n");
      exit(-1);
   }
   long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
   return milliseconds;
}

void Timer (Pipes p) {
	long long begin = 0, end = 0;
	long long timer;

	while (1) {
		PARAM_TYPE a;
		read(p.pipe1[0], &a, sizeof(PARAM_TYPE));
		switch (a) {
			case START:
				begin = CurrentTime();
				end = begin;
				break;
			case STOP:
				end = CurrentTime();
				break;
			case TIME:
				timer = end - begin;
				write(p.pipe2[1], &timer, sizeof(long long));
				break;
			case CLOSE:
				return;
			default:
				fprintf(stderr, "incorrect command, try again\n");
				break;
		}
	}
}

int main () {
	signal(SIGINT, termination);
	signal(SIGSEGV, termination);
	printf("Starting server...\n");
	void* context = zmq_ctx_new(); //создаем новый контекст
	if (!context) {
		fprintf(stderr, "zmq_ctx_new error\n");
		exit(-1);
	}
	void* respond = zmq_socket(context, ZMQ_PAIR); // создаем сокет в указанном контексте и возвращаем дескриптор во вновь созданный сокет.
	if (!respond) {
		fprintf(stderr, "zmq_socket error\n");
		exit(-1);
	}
	zmq_bind(respond, "tcp://*:4040"); // создаем конечную точку для приема подключений и привязываем ее к сокету, на который ссылается respond
	
	Init(&tree, -1, 0, -1, NULL, NULL);
	long long timer;
	ERROR_TYPE result;
	while (1) {
		Message mess;
		zmq_recv(respond, &mess, sizeof(Message), 0); // сообщение из respond сохраняем в mess

		Pipes arg;
		pid_t pid;
		int ping ;
		Tree* tmp;
		switch (mess.command) {
			case CREATE:
				if (pipe(arg.pipe1) == -1) {
					fprintf(stderr, "pipe1 error");
					exit(-1);
				}
				if (pipe(arg.pipe2) == -1) {
					fprintf(stderr, "pipe2 error");
					exit(-1);
				}
				pid = fork();
				switch (pid) {
					case -1:
						fprintf(stderr, "fork error\n");
						exit(-1);
					case 0:
						Timer(arg);
						return 0;
					default:
						break;
				}
				result = Add(tree, mess.parent, mess.id, pid, arg.pipe1, arg.pipe2);
				if (result == SUCCESS) {
					printf("created new process with id = %d and pid = %d\n", mess.id, pid);
				}
				break;
			case REMOVE:
				result = DeleteNode(tree, mess.id);
				if (result == SUCCESS) {
					printf("deleted node with id = %d and pid = %d\n", mess.id, pid);
				}
				break;
			case EXEC:
				tmp = Find(tree, mess.id);
				if (!tmp) {
					result = NODE_NOT_FOUND;
				} else if (waitpid(tmp->pid, NULL, WNOHANG) != 0) {
					result = NODE_IS_UNAVAILABLE;
				} else {
					result = SUCCESS;
					write(tmp->pipe1[1], &mess.param, sizeof(PARAM_TYPE));
					if (mess.command == EXEC && mess.param == TIME) {
						if (read(tmp->pipe2[0], &timer, sizeof(long long)) != sizeof(long long)) {
							result = READ_ERROR;
						}
					}
				}
				break;
			case PING:
				tmp = Find(tree, mess.id);
				result = SUCCESS;
				if (!tmp) {
					result = NODE_NOT_FOUND;
				} else if ((ping = waitpid(tmp->pid, NULL, WNOHANG)) != 0) {
					ping = 0;
				} else {
					ping = 1;
				}
				break;
			case EXIT:
				break;
			case UNKNOWN_COMM:
				//fprintf(stderr, "unknown command, try again\n");
				break;
		}
		if (mess.command == EXIT) {break;}

		zmq_send(respond, (void*)&result, sizeof(ERROR_TYPE), 0); // ставим в очередь сообщение, созданное из буфера, на который ссылаются аргументы result и sizeof
		if (result == SUCCESS) {
			switch (mess.command) {
				case CREATE:
					zmq_send(respond, &pid, sizeof(int),0);
					break;
				case REMOVE:
					break;
				case EXEC:
					if (mess.param == TIME) {
						zmq_send(respond, &timer, sizeof(long long),0);
					}
					break;
				case PING:
					zmq_send(respond, &ping, sizeof(int),0);
					break;
				default:
					break;
			}
		}
	}
	printf("Closing...\n");
	Destroy(tree);
	zmq_close(respond); // уничтожаем сокет, на который ссылается respond
	zmq_ctx_destroy(context); // уничтожаем context
	
	return 0;
}