#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Mess.h"

typedef struct Tree {
    int parent_id, id;
    pid_t pid;
    int pipe1[2], pipe2[2];
    struct Tree* son;
    struct Tree* brother;
} Tree;

void Init (Tree** node, int parent_id, int node_id, pid_t pid, int pipe1[2], int pipe2[2]);
Tree* Find (Tree* root, int id);
ERROR_TYPE Add (Tree* root, int id_parent, int id_child, pid_t pid, int pipe1[2], int pipe2[2]);
int GetTid (Tree* root, int id);
ERROR_TYPE DeleteNode (Tree* root, int id);
void Destroy (Tree* root);

#endif