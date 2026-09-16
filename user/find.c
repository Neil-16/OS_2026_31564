#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

int match(char *, char *);

char *
basename(char *path)
{
  char *p;

  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void
run_match(char *path, char **cmd)
{
  char *nargv[MAXARG];
  int i;
  int pid;

  if (cmd == 0) {
    printf("%s\n", path);
    return;
  }

  pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0) {
    for (i = 0; cmd[i] != 0; i++) {
      if (i + 2 >= MAXARG) {
        fprintf(2, "find: too many arguments\n");
        exit(1);
      }
      nargv[i] = cmd[i];
    }
    nargv[i] = path;
    nargv[i + 1] = 0;
    exec(nargv[0], nargv);
    fprintf(2, "find: exec %s failed\n", nargv[0]);
    exit(1);
  }

  wait(0);
}

void
find(char *path, char *name, char **cmd)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (match(name, basename(path)))
    run_match(path, cmd);

  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;
      find(buf, name, cmd);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char *cmd[MAXARG];
  int i;

  if (argc < 3) {
    fprintf(2, "usage: find <directory> <name> [-exec cmd ...]\n");
    exit(1);
  }

  if (argc == 3) {
    find(argv[1], argv[2], 0);
    exit(0);
  }

  if (strcmp(argv[3], "-exec") != 0 || argc < 5) {
    fprintf(2, "usage: find <directory> <name> [-exec cmd ...]\n");
    exit(1);
  }

  for (i = 4; i < argc && i - 4 < MAXARG - 1; i++)
    cmd[i - 4] = argv[i];
  cmd[i - 4] = 0;

  find(argv[1], argv[2], cmd);
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

static int matchhere(char *, char *);
static int matchstar(int, char *, char *);

int
match(char *re, char *text)
{
  if (re[0] == '^')
    return matchhere(re + 1, text);
  do { // must look at empty string
    if (matchhere(re, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
static int
matchhere(char *re, char *text)
{
  if (re[0] == '\0')
    return 1;
  if (re[1] == '*')
    return matchstar(re[0], re + 2, text);
  if (re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    return matchhere(re + 1, text + 1);
  return 0;
}

// matchstar: search for c*re at beginning of text
static int
matchstar(int c, char *re, char *text)
{
  do { // a * matches zero or more instances
    if (matchhere(re, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}
