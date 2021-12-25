#include "Tree.h"

void Init (Tree** node, int parent_id, int node_id, pid_t pid, int pipe1[2], int pipe2[2]) {
    (*node) = malloc(sizeof(Tree));
    if (!(*node)) {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    (*node)->parent_id = parent_id;
    (*node)->id = node_id;
    (*node)->pid = pid;
    if (pipe1 && pipe2) {
        memcpy((*node)->pipe1, pipe1, sizeof(int) * 2);
        memcpy((*node)->pipe2, pipe2, sizeof(int) * 2);
    }
    (*node)->son = NULL;
    (*node)->brother = NULL;
}

Tree* Find (Tree* root, int id) {
    if (!root) {
        return NULL;
    }
    if (root->id == id) {
        return root;
    }
    root = root->son;
    while (root) {
        Tree* tmp = root;
        tmp = Find(tmp, id);
        if (tmp) {
            return tmp;
        } else {
            root = root->brother;
        }
    }
    return NULL;
}

ERROR_TYPE Add (Tree* root, int parent_id, int child_id, pid_t pid, int pipe1[2], int pipe2[2]) {
    if (Find(root, child_id)) {
        return ALREADY_EXIST;
    }
    Tree* parent = Find(root, parent_id);
    if (!parent) {
        return PARENT_NOT_FOUND;
    }
    Tree* newEl;
    Init(&newEl, parent_id, child_id, pid, pipe1, pipe2);
    Tree* son = parent->son;
    if (!son) {
        parent->son = newEl;
    } else {
        while (son->brother) {
            son = son->brother;
        }
        son->brother = newEl;
    }
    return SUCCESS;
}

pid_t GetPid (Tree* root, int id) {
    pid_t pid = 0;
    Tree* tmp = Find(root, id);
    if (tmp) {
        pid = tmp->pid;
    }
    return pid;
}

ERROR_TYPE DeleteNode (Tree* root, int id) {
    Tree* tmp = Find(root, id);
    if (!tmp) {
        return NODE_NOT_FOUND;
    }
    PARAM_TYPE param = CLOSE;
    if (tmp->pid != -1) {
        write(tmp->pipe1[1], &param, sizeof(PARAM_TYPE));
        waitpid(tmp->pid, NULL, 0);
        close(tmp->pipe1[0]);
        close(tmp->pipe1[1]);
        close(tmp->pipe2[0]);
        close(tmp->pipe2[1]);
    }
    if (!tmp->son && !tmp->brother) {
        Tree* parent = Find(root, tmp->parent_id);
        if (parent->son == tmp) {
            parent->son = NULL;
        } else {
            parent = parent->son;
            while (parent->brother != tmp) {
                parent = parent->brother;
            }
            parent->brother = NULL;
        }
        free(tmp);
    } else {
        Tree* last = tmp;
        Tree* parent = NULL;
        int rrr = 0;
        while (last->brother) {
            parent = last;
            last = last->brother;
        }
        while (last->son) {
            parent = last;
            last = last->son;
            rrr = 1;
        }
        *tmp = *last;
        free(last);
        if (!rrr) {
            parent->brother = NULL;
        } else {
            parent->son = NULL;
        }
    }
    return SUCCESS;
}

void Destroy (Tree* root) {
    if (!root) {
        return;
    }
    Destroy(root->brother);
    Destroy(root->son);
    PARAM_TYPE param = CLOSE;
    if (root->pid != -1) {
        write(root->pipe1[1], &param, sizeof(PARAM_TYPE));
        waitpid(root->pid, NULL, 0);
        close(root->pipe1[0]);
        close(root->pipe1[1]);
        close(root->pipe2[0]);
        close(root->pipe2[1]);
    }
    free(root);
}