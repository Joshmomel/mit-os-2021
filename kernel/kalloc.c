// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct 
{
  struct spinlock lock;
  struct run *freelist;
} kmem_freelists[NCPU];

void
kinit()
{
  for (int i = 0; i < NCPU; i++)
  {
    initlock(&kmem_freelists[i].lock, "kmem");
    kmem_freelists[i].freelist = 0;
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int cup_id = cpuid();

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem_freelists[cup_id].lock);
  r->next = kmem_freelists[cup_id].freelist;
  kmem_freelists[cup_id].freelist = r;
  release(&kmem_freelists[cup_id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cup_id = cpuid();

  acquire(&kmem_freelists[cup_id].lock);
  r = kmem_freelists[cup_id].freelist;
  if (r)
  {
    kmem_freelists[cup_id].freelist = r->next;
  } else {
    // steal free list from other cpu
    for (int i = 0; i < NCPU; i++)
    {
      if (i == cup_id)
        continue;
      acquire(&kmem_freelists[i].lock);
      r = kmem_freelists[i].freelist;
      if (r)
      {
        kmem_freelists[i].freelist = r->next;
        release(&kmem_freelists[i].lock);
        break;
      }
      release(&kmem_freelists[i].lock);
    }
  }

  release(&kmem_freelists[cup_id].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
