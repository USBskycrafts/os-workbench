#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>


#define COMM_SIZE 1024

typedef struct node {
  char comm[COMM_SIZE];
  struct node* children[0];
}node;

void set_node(pid_t id) {
  char* id_s;
  asprintf(&id_s, "%d", id);
  char* path;
  sprintf(path, "/proc/%s/stat", id_s);
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
