#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Usage: sandbox mask path cmd [args...]\n");
    exit(1);
  }

  int mask = atoi(argv[1]);
  char *path = argv[2];

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    if (interpose(mask, path) < 0) {
      fprintf(2, "interpose failed\n");
      exit(1);
    }
    exec(argv[3], &argv[3]);
    fprintf(2, "exec failed\n");
    exit(1);
  } else {
    // Parent process
    int status;
    wait(&status);
    exit(0);
  }
}