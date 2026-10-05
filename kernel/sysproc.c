#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Return the PTE for the given user virtual address, so that
// user space can inspect its own page table.  Returns 0 if the
// address has no PTE.
uint64
sys_pgpte(void)
{
  uint64 va;
  pte_t *pte;

  argaddr(0, &va);
  if ((pte = walk(myproc()->pagetable, va, 0)) == 0)
    return 0;
  return *pte;
}

// Print this process's page table.  vmprint() in vm.c is part of
// the lab; until it is written this does nothing.
uint64
sys_vmprint(void)
{
  vmprint(myproc()->pagetable);
  return 0;
}

// TODO(pgtbl lab): implement pgaccess() here.
//
// Report which of the num pages starting at va have been accessed
// since the last call, as a bitmask copied to the user buffer at
// buf (first page in the least significant bit).  Return -1 for
// invalid arguments, including a page that is not mapped.
// Remember to clear PTE_A on each page you report, and to define
// PTE_A in riscv.h.
uint64
sys_pgaccess(void)
{
  panic("sys_pgaccess not implemented");
}

// Return 1 if the kernel page table contains at least one superpage
// (a 2MB leaf at level 1), 0 if it does not.
//
// Superpage support in kvmmap() is part of the pgtbl lab; the check
// in ksuper() is scaffolding and is already written.
uint64
sys_ksuper(void)
{
  return ksuper();
}
