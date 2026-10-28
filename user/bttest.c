//
// test program for backtrace().
// calls pause(), which the kernel makes call backtrace().
//

#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  pause(1);
  exit(0);
}
