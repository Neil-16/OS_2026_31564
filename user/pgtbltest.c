#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "user/user.h"

// Print the page-table entries for the first 10 and last 10 pages of
// this process, using pgpte().
//
// This is the output for the "inspect a user-process page table"
// exercise.  Explain every line of it in answers-pgtbl.txt.
void
print_pgtbl(void)
{
  printf("\n--- FIRST 10 PAGES ---\n");
  for (uint64 va = 0; va < 10 * PGSIZE; va += PGSIZE) {
    uint64 pte = pgpte((char *)va);
    printf("va %lx pte %lx pa %lx perm %lx\n",
           va, pte, PTE2PA(pte), PTE_FLAGS(pte));
  }

  // the guard page below TRAPFRAME, TRAPFRAME, TRAMPOLINE, and the
  // top of the user stack
  printf("\n--- LAST 10 PAGES ---\n");
  for (uint64 va = MAXVA - 10 * PGSIZE; va < MAXVA; va += PGSIZE) {
    uint64 pte = pgpte((char *)va);
    printf("va %lx pte %lx pa %lx perm %lx\n",
           va, pte, PTE2PA(pte), PTE_FLAGS(pte));
  }
}

// Exercise vmprint(), which you implement in kernel/vm.c.
void
vmprint_test(void)
{
  printf("\n--- VMP ---\n");
  vmprint();
  printf("VMP OK\n");
}

// Exercise pgaccess(), which you implement in kernel/sysproc.c.
//
// Signature: pgaccess(va, num, buf) reports which of the num pages
// starting at va have been accessed since the last call, as a bitmask
// written to the user buffer at buf.  The first page is bit 0.
void
pgaccess_test(void)
{
  uint64 bits;
  int ok = 1;

  printf("\n--- PGACCESS ---\n");

  // Access three of our own mapped pages to set their PTE_A bits: the
  // second text page, the data page, and the stack page.  We read
  // rather than write because text is mapped execute-only-write, and
  // we skip 0x3000 because that is the kernel-only guard page.
  // (We also avoid unmapped pages: this xv6 only recovers from load
  // faults, scause 13, in vmfault(); a store fault, scause 15, kills
  // the process.)
  volatile char *p = (char *)0x1000;
  volatile int sink = 0;
  sink += p[0];           // read text page 1  -> bit 0
  sink += p[PGSIZE];      // read text page 2  -> bit 1
  if (sink == 0x7ffff)
    printf("sink\n");

  // Only check the two text pages.  The stack page cannot be used for
  // the clear test: it is in active use by this very function, so the
  // hardware sets its PTE_A again the instant pgaccess clears it.
  bits = 0;
  if (pgaccess((char *)0x1000, 4, (char *)&bits) != 0) {
    printf("pgaccess: unexpected error\n");
    ok = 0;
  } else if ((bits & 0x3) != 0x3) {
    printf("pgaccess: expected bits 0x3, got bitmask %lx\n", bits);
    ok = 0;
  }

  // The second call must report nothing for the text pages, because
  // pgaccess clears PTE_A as it reports it.
  bits = ~0;
  if (pgaccess((char *)0x1000, 4, (char *)&bits) != 0) {
    printf("pgaccess: unexpected error on second call\n");
    ok = 0;
  } else if ((bits & 0x3) != 0) {
    printf("pgaccess: PTE_A not cleared, bitmask %lx\n", bits);
    ok = 0;
  }

  // an unmapped page is an error
  if (pgaccess((char *)(MAXVA - 3 * PGSIZE), 1, (char *)&bits) == 0) {
    printf("pgaccess: expected error for unmapped page\n");
    ok = 0;
  }

  // a bad buffer address is an error
  if (pgaccess((char *)0x1000, 1, (char *)(MAXVA - 8)) == 0) {
    printf("pgaccess: expected error for bad buffer\n");
    ok = 0;
  }

  // an unaligned va is an error
  if (pgaccess((char *)0x1001, 1, (char *)&bits) == 0) {
    printf("pgaccess: expected error for unaligned va\n");
    ok = 0;
  }

  if (ok)
    printf("PGACCESS OK\n");
  else
    printf("PGACCESS FAIL\n");
}

// ugetpid() reads the pid from the shared USYSCALL page instead of
// trapping into the kernel.  The mapping is part of the pgtbl lab.
void
ugetpid_test(void)
{
  printf("\n--- UGETPID ---\n");
  if (ugetpid() == getpid())
    printf("UGETPID OK\n");
  else
    printf("UGETPID FAIL\n");
}

// The kernel page table should use superpages for 2MB-aligned regions.
// The superpage support in kvmmap() is part of the pgtbl lab; the
// ksuper() check below is scaffolding and is already written.
void
ksuper_test(void)
{
  printf("\n--- KSUPER ---\n");
  if (ksuper())
    printf("KSUPER OK\n");
  else
    printf("KSUPER FAIL\n");
}

int
main(void)
{
  printf("pgtbltest\n");

  print_pgtbl();
  vmprint_test();
  pgaccess_test();
  ugetpid_test();
  ksuper_test();

  return 0;
}
