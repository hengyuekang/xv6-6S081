#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 begin;
  if(argaddr(0, &begin) < 0)
    return -1;
  int num;
  if(argint(1,&num)<0)
    return -1;
  if(num>32)
  {
    printf("pgacess:too many pages to be scaned.\n");
    return -1;
  }
  unsigned int res=0;
  for(int i=0;i<num;i++)
  {
    pte_t *pte=walk(myproc()->pagetable,begin+i*PGSIZE,1);
    if((*pte)& PTE_V)
    {
      if((*pte)& PTE_A)
      {
        res=(res)|(1<<i);
        // printf("pos in buf :%d,a accessed pte:%p,and res now is:%p\n",i,*pte,res);
        (*pte)=(*pte)-PTE_A;
        // printf("pte clear PTE_A :%p\n",*pte);
      }
    }
  }
  uint64 dest;
  if(argaddr(2,&dest)<0)
    return -1;
  // int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
  // printf("res(copyout to bitmask)=%p\n",res);
  copyout(myproc()->pagetable,dest,(char*)&res,4);
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
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
