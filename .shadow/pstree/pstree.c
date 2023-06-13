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

/*
* TO DO:
* 1. work list is needed when generating node because we can get child before parent
* 
*/

#define NODE_SIZE 4096

struct node {
  pid_t pid, ppid;
  const char* comm;
  off_t size;
  off_t children[0];
};

struct node* nodes[NODE_SIZE];
static off_t offset;

off_t craete_node(pid_t pid) {
  char* pid_s;
  asprintf(&pid_s, "%d", pid);
  char* path;
  asprintf(&path, "/proc/%s/stat", pid_s);
  pid_t ppid;
  char comm[512];
  char state;
  FILE* f = fopen(path, "r");
  fscanf(f, "%d %s %c %d", &pid, comm, &state, &ppid);
  fclose(f);
  off_t o = ++offset;
  struct node* n = (struct node*)malloc(sizeof(struct node));
  n->pid = pid;
  n->ppid = ppid;
  n->comm = malloc(sizeof(comm));
  memcpy((void*)n->comm, comm, sizeof(comm));
  nodes[o] = n;
  return o;
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
