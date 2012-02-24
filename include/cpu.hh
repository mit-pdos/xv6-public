#pragma once

#include "mmu.h"
#include "atomic.hh"

struct cilkframe;
using std::atomic;

extern atomic<u64> tlbflush_req;

// Per-CPU state
struct cpu {
  u8 id;                       // Local APIC ID; index into cpus[] below
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct context *scheduler;   // swtch() here to enter scheduler
  struct cilkframe *cilkframe;
  int timer_printpc;
  atomic<u64> tlbflush_done;   // last tlb flush req done on this cpu

  // Cpu-local storage variables; see below
  struct cpu *cpu;
  struct proc *proc;           // The currently-running process.
  struct kmem *kmem;           // The per-core memory table
} __mpalign__;

extern struct cpu cpus[NCPU];

// Per-CPU variables, holding pointers to the
// current cpu and to the current process.
// XXX(sbw) asm labels default to RIP-relative and
// I don't know how to force absolute addressing.
static inline struct cpu *
mycpu(void)
{
  u64 val;
  __asm volatile("movq %%gs:0, %0" : "=r" (val));
  return (struct cpu *)val;
}

static inline struct proc *
myproc(void)
{
  u64 val;
  __asm volatile("movq %%gs:8, %0" : "=r" (val));
  return (struct proc *)val;
}

static inline struct kmem *
mykmem(void)
{
  u64 val;
  __asm volatile("movq %%gs:16, %0" : "=r" (val));
  return (struct kmem *)val;
}
