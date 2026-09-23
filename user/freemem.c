#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  uint64 m = freemem();
  printf("free memory: %d bytes (%d pages)\n", (int)m, (int)(m / 4096));
  exit(0);
}