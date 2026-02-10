#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "syscall.h" 
#include"proc.h"
 

void pid_ns_decref(struct pid_namespace *ns);
void mount_ns_decref(struct mount_namespace *ns);
void uts_ns_decref(struct uts_namespace *ns);
void ipc_ns_decref(struct ipc_namespace *ns);

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  struct proc *p = myproc();
  if(p->vpid > 0)
    return p->vpid;
  return p->pid;
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

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
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
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
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

//changed 

uint64
sys_unshare(void)
{
  int flags;
  argint(0, &flags);

  struct proc *p = myproc();

  if(flags & 0x4) { // CLONE_NEWUTS
    // ۱. ساخت یک فضای کاملاً مستقل
    struct uts_namespace *old_ns = p->uts_ns;
    struct uts_namespace *new_ns = (struct uts_namespace *)kalloc();
    
    if(new_ns == 0) return -1;

    safestrcpy(new_ns->hostname, old_ns->hostname, 64);
    new_ns->refcount = 1;

    p->uts_ns = new_ns;

  
  }
  
  // --- PID Namespace Isolation ---
  if(flags & CLONE_NEWPID){
  struct pid_namespace *ns = kalloc();
  if(ns == 0)
    return -1;

  ns->refcount = 1;
  ns->next_pid = 2;   

  p->pid_ns = ns;
  p->vpid = 1;       
}

  // 3. Mount Namespace
  if(flags & CLONE_NEWNS){
  struct mount_namespace *new_mnt = (struct mount_namespace*)kalloc();
  if(new_mnt == 0) return -1;

  new_mnt->refcount = 1;
    begin_op();
  if(p->mount_ns && p->mount_ns->root){
    new_mnt->root = idup(p->mount_ns->root); // افزایش رفرنس inode
  } else {
  }
  end_op();

  p->mount_ns = new_mnt;
}

// 4. IPC Namespace
if(flags & CLONE_NEWIPC){
  struct ipc_namespace *new_ipc = (struct ipc_namespace*)kalloc();
  if(new_ipc == 0) return -1;
  new_ipc->refcount = 1;
  p->ipc_ns = new_ipc;
}
  return 0;
}

// در kernel/sysproc.c
uint64
sys_sethostname(void)
{
  char name[64];
  int len;
  argstr(0, name, 64);
  argint(1, &len);

  struct proc *p = myproc();
  safestrcpy(p->uts_ns->hostname, name, sizeof(p->uts_ns->hostname));
  return 0;
}

uint64
sys_gethostname(void)
{
  uint64 addr;
  int len;
  argaddr(0, &addr);
  argint(1, &len);

  struct proc *p = myproc();
  if(p->uts_ns && addr != 0) {
    if(copyout(p->pagetable, addr, p->uts_ns->hostname, len) < 0)
      return -1;
    return 0;
  }
  return -1;
}

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->trace_mask = mask;
  return 0;
}
