#include "kernel/types.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1
#define PRIMES 35

void find_prime(int *p_left)
{
  int n;
  int p_right[2];

  close(p_left[WRITEEND]);
  int read_num = read(p_left[READEND], &n, sizeof(int));
  if (read_num == 0)
  {
    exit(0);
  }

  pipe(p_right);
  if (fork() == 0)
  {
    find_prime(p_right);
  }
  else
  {
    // parent
    close(p_right[READEND]);
    printf("prime %d\n", n);
    int prime = n;
    while (read(p_left[READEND], &n, sizeof(int)) != 0)
    {
      if (n % prime != 0)
      {
        write(p_right[WRITEEND], &n, sizeof(int));
      }
    }
    close(p_right[WRITEEND]);
    wait((int *)0);
    exit(0);
  }

  printf("prime %d\n", n);
}

int main()
{
  int p_left[2];
  pipe(p_left);

  if (fork() == 0)
  {
    // child
    find_prime(p_left);
  }
  else
  {
    // parent
    for (int i = 2; i <= PRIMES; i++)
    {
      write(p_left[WRITEEND], &i, sizeof(int));
    }
    close(p_left[WRITEEND]);
    wait((int *)0);
  }

  exit(0);
}
