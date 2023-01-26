#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int argv_len = 0;
  char buf[128] = {'\0'};
  char *new_argv[MAXARG];

  for (int i = 1; i < argc; i++)
  {
    new_argv[i - 1] = argv[i];
  }

  while (gets(buf, sizeof(buf)))
  {
    int buf_len = strlen(buf);
    if (buf_len < 1)
    {
      break;
    }
    buf[buf_len - 1] = '\0';
    argv_len = argc - 1;

    for (char *p = buf; *p; p++)
    {
      while (*p && (*p == ' '))
      {
        *p++ = '\0';
      }
      if (*p)
      {
        new_argv[argv_len++] = p;
      }
      while (*p && (*p != ' '))
      {
        p++;
      }
    }

    new_argv[argv_len] = "\0";

    if (fork() == 0)
    {
      // child
      printf("exec start: \n");
      exec(new_argv[0], new_argv);
    }
    else
    {
      wait(0);
    }
  }

  exit(0);
}
