#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

static uint16
load16(char *data)
{
  return (uint16)(uchar)data[0] |
         ((uint16)(uchar)data[1] << 8);
}

static uint32
load32(char *data)
{
  return (uint32)(uchar)data[0] |
         ((uint32)(uchar)data[1] << 8) |
         ((uint32)(uchar)data[2] << 16) |
         ((uint32)(uchar)data[3] << 24);
}

static uint64
load64(char *data)
{
  uint64 value;
  int i;

  value = 0;
  for (i = 0; i < 8; i++)
    value |= (uint64)(uchar)data[i] << (8 * i);
  return value;
}

void
memdump(char *fmt, char *data, int len)
{
  int offset;
  int remaining;
  int i;
  uint16 value16;
  uint32 value32;
  uint64 value64;

  offset = 0;
  for (i = 0; fmt[i] != 0; i++) {
    remaining = len - offset;
    if ((fmt[i] == 'i' && remaining < 4) ||
        (fmt[i] == 'p' && remaining < 8) ||
        (fmt[i] == 'h' && remaining < 2) ||
        (fmt[i] == 'c' && remaining < 1) ||
        (fmt[i] == 's' && remaining < 8)) {
      printf("memdump: not enough data for '%c'\n", fmt[i]);
      return;
    }

    switch (fmt[i]) {
    case 'i':
      value32 = load32(data + offset);
      printf("%d\n", (int)value32);
      offset += 4;
      break;
    case 'p':
      value64 = load64(data + offset);
      printf("%lx\n", value64);
      offset += 8;
      break;
    case 'h':
      value16 = load16(data + offset);
      printf("%d\n", (int)(short)value16);
      offset += 2;
      break;
    case 'c':
      printf("%c\n", (uint32)(uchar)data[offset]);
      offset++;
      break;
    case 's':
      value64 = load64(data + offset);
      printf("%s\n", (char *)value64);
      offset += 8;
      break;
    case 'S':
      while (offset < len && data[offset] != 0) {
        printf("%c", (uint32)(uchar)data[offset]);
        offset++;
      }
      printf("\n");
      return;
    }
  }
}
