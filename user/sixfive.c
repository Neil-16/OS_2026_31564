#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char *seps = " -\r\t\n./,";

void
maybe_print(int have, int val)
{
  if (have && (val % 5 == 0 || val % 6 == 0))
    printf("%d\n", val);
}

void
sixfive(int fd)
{
  char c;
  int n;
  int val;
  int isnum;
  int have;

  val = 0;
  isnum = 1;
  have = 0;

  while ((n = read(fd, &c, 1)) == 1) {
    if (strchr(seps, c)) {
      maybe_print(have && isnum, val);
      val = 0;
      isnum = 1;
      have = 0;
    } else if (c >= '0' && c <= '9') {
      if (isnum) {
        have = 1;
        val = val * 10 + (c - '0');
      }
    } else {
      isnum = 0;
      have = 0;
    }
  }

  maybe_print(have && isnum, val);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if (argc <= 1) {
    sixfive(0);
    exit(0);
  }

  for (i = 1; i < argc; i++) {
    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("sixfive: cannot open %s\n", argv[i]);
      exit(1);
    }
    sixfive(fd);
    close(fd);
  }
  exit(0);
}
