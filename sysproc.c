#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu
  
  return 0;
}

extern int sched_trace_enabled;
extern int sched_trace_counter;
int sys_enable_sched_trace(void)
{
  if (argint(0, &sched_trace_enabled) < 0)
  {
    cprintf("enable_sched_trace() failed!\n");
  }
  
  sched_trace_counter = 0;

  return 0;
}

extern int fork_alternation;
int sys_fork_alternate_winner(void) {
  
  int state;

  if(argint(0, &state) < 0) { //invalid input
    return -1;
  }

  if (state == 0 || state == 1) {
    fork_alternation = state;
    return 0; //success
  }

  return -1; //invalid input above 1
}

extern int stride_sched;
int sys_set_sched(void) {
  int stride;

  if (argint(0, &stride) < 0) { //invalid
    return -1;
  }

  if (stride == 0 || stride == 1) {
    stride_sched = stride;
    return 0;
  }

  return -1; //invalide input above 1
}

int sys_tickets_owned(void) {
  //check each process for pid
  //when pid match get tickets with p->tickets
  int passid;
  if(argint(0, &passid) < 0) { //pid error
    return -1;
  }
  int retval = tickets_owned(passid);
  return retval; 
}

int sys_transfer_tickets(void) {
  //can use current process with myproc()
  //find process with PID
  //transfer tickets
  int passid;
  int tickets_transfered;
  if (argint(0, &passid) < 0) { //pid error
    cprintf("\tpassid: %d\n", passid);
    return -3;
  }
  if (argint(1, &tickets_transfered) < 0) {
    cprintf("\tpassid: %d\n", tickets_transfered);
    return -1;
  }
  if (tickets_transfered >= myproc()->tickets) { //ticket error, more than able. Must have 1 at least
    return -2;
  }
  int retval = transfer_tickets(passid, tickets_transfered);
  return retval;
}