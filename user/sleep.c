#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 1)
  {
    printf("error: need to pass argument\n");
  }
  
  sleep(atoi(argv[1]));
  exit(0);
}