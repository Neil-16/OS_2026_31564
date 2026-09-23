#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXSCAN (256 * 4096)

static int
isalnumc(char c)
{
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9');
}

int
main(void)
{
  char *base = sbrk(MAXSCAN);
  if (base == (char *)-1)
    exit(1);

  for (int i = 0; i < MAXSCAN; i++) {
    if (!isalnumc(base[i]))
      continue;
    int start = i;
    while (i < MAXSCAN && isalnumc(base[i]))
      i++;
    int len = i - start;
    if (len < 4 || len > 128)
      continue;
    char word[129];
    for (int k = 0; k < len; k++)
      word[k] = base[start + k];
    word[len] = '\0';
    if (strcmp(word, "attack") == 0 || strcmp(word, "secret") == 0)
      continue;
    printf("%s\n", word);
    exit(0);
  }
  exit(0);
}