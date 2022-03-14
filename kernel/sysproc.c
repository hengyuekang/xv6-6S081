#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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

uint64
sys_sigalarm(void)
{
  int interval;

  if (argint(0, &interval) < 0)
    return -1;
  uint64 fn;
  if (argaddr(1, &fn) < 0)
    return -1;
  if (interval <= 0 || fn == 0) // there are no NULL for xv6,and occasionally the address of periodic==0,so ignore if passed 0
    return 0;
  myproc()->ticks = interval;
  myproc()->handler = fn;
  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p=myproc();
  p->trapframe->epc = p->alarmcontext.epc;
  p->trapframe->ra = p->alarmcontext.ra;
  p->trapframe->sp = p->alarmcontext.sp;
  p->trapframe->gp = p->alarmcontext.gp;
  p->trapframe->tp = p->alarmcontext.tp;
  p->trapframe->s0 = p->alarmcontext.s0;
  p->trapframe->s1 = p->alarmcontext.s1;
  p->trapframe->s2 = p->alarmcontext.s2;
  p->trapframe->s3 = p->alarmcontext.s3;
  p->trapframe->s4 = p->alarmcontext.s4;
  p->trapframe->s5 = p->alarmcontext.s5;
  p->trapframe->s6 = p->alarmcontext.s6;
  p->trapframe->s7 = p->alarmcontext.s7;
  p->trapframe->s8 = p->alarmcontext.s8;
  p->trapframe->s9 = p->alarmcontext.s9;
  p->trapframe->s10 = p->alarmcontext.s10;
  p->trapframe->s11 = p->alarmcontext.s11;
  p->trapframe->t0 = p->alarmcontext.t0;
  p->trapframe->t1 = p->alarmcontext.t1;
  p->trapframe->t2 = p->alarmcontext.t2;
  p->trapframe->t3 = p->alarmcontext.t3;
  p->trapframe->t4 = p->alarmcontext.t4;
  p->trapframe->t5 = p->alarmcontext.t5;
  p->trapframe->t6 = p->alarmcontext.t6;
  p->trapframe->a0 = p->alarmcontext.a0;
  p->trapframe->a1 = p->alarmcontext.a1;
  p->trapframe->a2 = p->alarmcontext.a2;
  p->trapframe->a3 = p->alarmcontext.a3;
  p->trapframe->a4 = p->alarmcontext.a4;
  p->trapframe->a5 = p->alarmcontext.a5;
  p->trapframe->a6 = p->alarmcontext.a6;
  p->trapframe->a7 = p->alarmcontext.a7;
  p->handling = 0;
  return 0;
}
