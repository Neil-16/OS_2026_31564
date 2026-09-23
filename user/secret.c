#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: secret string\n");
    exit(1);
  }

  int len = strlen(argv[1]);
  for (int p = 0; p < 8; p++) {
    char *mem = sbrk(4096);
    if (mem == (char *)-1)
      exit(1);
    for (int i = 0; i <= len; i++)
      mem[i] = argv[1][i];
  }

  exit(0);
}