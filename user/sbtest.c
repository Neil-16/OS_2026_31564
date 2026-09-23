#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: sbtest file\n");
    exit(1);
  }

  int fd = open(argv[1], 0);
  if (fd < 0) {
    fprintf(2, "cannot open %s\n", argv[1]);
    exit(1);
  }
  
  char buf[512];
  int n = read(fd, buf, sizeof(buf));
  if (n > 0) {
    write(1, buf, n);
  }
  close(fd);
  exit(0);
}