#define _GNU_SOURCE
#include <stddef.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>


#define NODE_SIZE 4096

struct node {
  pid_t pid, ppid;
  const char* comm;
  unsigned size;
  off_t children[0];
};

struct node* nodes[NODE_SIZE];
static off_t offset;



off_t create_node(pid_t pid) {
  char* pid_s;
  asprintf(&pid_s, "%d", pid);
  char* path;
  asprintf(&path, "/proc/%s/stat", pid_s);
  pid_t ppid;
  char comm[513];
  char state;
  FILE* f = fopen(path, "r");
  fscanf(f, "%d %512s %c %d", &pid, comm, &state, &ppid);
  fclose(f);
  off_t o = ++offset;
  struct node* n = (struct node*)malloc(sizeof(struct node));
  n->pid = pid;
  n->ppid = ppid;
  n->comm = malloc(sizeof(comm));
  memcpy((void*)n->comm, comm, sizeof(comm));
  n->size = 0;
  nodes[o] = n;
  return o;
}


// scan the proc directory
void recur_scan() {
  DIR* dir = opendir("proc");
  struct dirent* ptr;
  while((ptr = readdir(dir)) != NULL) {
    if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
        continue;
    }
    char* num;
    asprintf(&num, "%s", ptr->d_name);
    char* next;
    int pid = strtol(num, &next, 10);
    if((next == num) || (*next != '\0')) {
      continue;
    }
    create_node(pid);
  }
  closedir(dir);
}

// build the node tree
void build_tree() {
  for(off_t i = 0; i < offset; i++) {
    for(off_t j = 0; j < offset; j++) {
      if(i == j) {
        continue;
      }
      if(nodes[j]->ppid == nodes[i]->pid) {
        nodes[i]->size++;
        nodes[i] = realloc(nodes[i], nodes[i]->size * sizeof(off_t) + sizeof(struct node));
        nodes[i]->children[nodes[i]->size - 1] = j;
      } else if(nodes[j]->pid == nodes[i]->ppid) {
        nodes[j]->size++;
        nodes[j] = realloc(nodes[j], nodes[j]->size * sizeof(off_t) + sizeof(struct node));
        nodes[j]->children[nodes[j]->size - 1] = i;
      }
    }
  }
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
  recur_scan();
  build_tree();
  return 0;
}
