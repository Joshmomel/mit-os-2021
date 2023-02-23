#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

static struct stats
{
  int ncopyin;
  int ncopyinstr;
} stats;

int copyin_new(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  struct proc *p = myproc();

  if (srcva >= p->sz || srcva + len >= p->sz || srcva + len < srcva)
    return -1;
  memmove((void *)dst, (void *)srcva, len);
  stats.ncopyin++; // XXX lock
  return 0;
}

int copyinstr_new(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  struct proc *p = myproc();
  char *s = (char *)srcva;

  stats.ncopyinstr++; // XXX lock
  for (int i = 0; i < max && srcva + i < p->sz; i++)
  {
    dst[i] = s[i];
    if (s[i] == '\0')
      return 0;
  }
  return -1;
}