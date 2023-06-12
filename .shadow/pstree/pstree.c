#define _GNU_SOURCE
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>

#define COMM_SIZE 1024

struct node {
  pid_t pid;
  pid_t ppid;
  char comm[COMM_SIZE];
  size_t children_size;
  size_t cur_size;
  struct node* children[0];
};

static struct node* head;

struct node* set_node(pid_t id) {
  char* id_s;
  asprintf(&id_s, "%d", id);
  char* path;
  asprintf(&path, "/proc/%s/stat", id_s);
  int pid, ppid;
  char state;
  char comm[COMM_SIZE];
  FILE* f = fopen(path, "r");
  fscanf(f, "%d %s %c %d", &pid, comm, &state, &ppid);
  // create node with 2 children
  struct node* cur = malloc(sizeof(struct node) + 2 * sizeof(struct node*));
  cur->children_size = 2;
  cur->cur_size = 0;
  // cpy comm
  strcpy(cur->comm, comm);
  cur->pid = pid;
  cur->ppid = ppid;
  return cur;
}


void node_init() {
  // find the first process(pid = 1)
  head = set_node(1);
}

struct node* search_node(struct node* cur, pid_t pid) {
  if(cur->pid == pid) {
    return cur;
  }
  assert(cur->cur_size > 0);
  for(int i = 0; i < cur->cur_size; i++) {
    struct node* ret;
    if((ret = search_node(cur->children[i], pid)) != NULL) {
      return ret;
    }
  }
  return NULL;
}

void insert_node(struct node* cur) {
  pid_t pid = cur->pid;
  pid_t ppid = cur->ppid;
  struct node* pnode = search_node(head, ppid);
  pnode->cur_size++;
  if(pnode->cur_size == pnode->children_size) {
    // enlarge the pnode to insert the child
    pnode = realloc((void*) pnode->children, pnode->children_size * 2 * sizeof(struct node*) + sizeof(struct node));
    pnode->children_size = 2 * pnode->children_size;
  }
  pnode->children[pnode->cur_size - 1] = cur;
}


int main(int argc, char *argv[]) {
  int opt;
  static const struct option opts[3] = {
    {"show-pids", no_argument, NULL, 'p'},
    {"numeric-sort", no_argument, NULL, 'n'},
    {"version", no_argument, NULL, 'v'}
  };
  while((opt = getopt_long(argc, argv, "p::n::v::", opts, NULL)) != -1) {
    switch(opt) {
    case 'p': {
      break;
    }
    case 'n': {
      break;
    }
    case 'v': {
      printf("pstree version 0.0.1\n");
      break;
    }
    case '?':
    default: {
      fprintf(stderr, "please check the arguments\n");
      exit(127);
    }
    }
  }
  assert(!argv[argc]);
  return 0;
}
