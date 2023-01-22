#include "kernel/types.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1

int main()
{
  int p1[2];
  int p2[2];
  int pid;
  char buf[1];

  // create a pipe, with two FDs in fds[0], fds[1].
  pipe(p1);
  pipe(p2);
  pid = fork();

  // p1 child write parent read
  // p2 parent write child read
  if (pid < 0)
  {
    exit(1);
  }
  else if (pid == 0)
  {
    // child
    close(p1[WRITEEND]);
    close(p2[READEND]);
    // read function is a block
    // will unblock when it actually read (
    // it waits the `write(p1[WRITEEND], " ", 1` to be done
    read(p1[READEND], buf, 1);
    printf("%d: received ping\n", getpid());
    close(p1[READEND]);

    write(p2[WRITEEND], " ", 1);
    close(p2[WRITEEND]);
  }
  else
  {
    // parent
    close(p1[READEND]);
    close(p2[WRITEEND]);

    write(p1[WRITEEND], " ", 1);
    close(p1[WRITEEND]);
    
    read(p2[READEND], buf, 1);
    printf("%d: received pong\n", getpid());
    close(p2[READEND]);
  }
  exit(0);
}