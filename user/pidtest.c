#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

int
main(void)
{
  int pid;

  printf("\n--- Starting PID Namespace Test (vpid-based) ---\n");

  // Parent PID (global)
  printf("1. Parent: getpid() = %d (global pid)\n", getpid());

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child
    printf("2. Child(before unshare): getpid() = %d (should be global)\n", getpid());

    // Create new PID namespace
    unshare(0x1); // CLONE_NEWPID

    int mypid = getpid();
    printf("3. Child(after unshare): getpid() = %d (should be 1)\n", mypid);

    // Fork inside new PID namespace
    int cpid = fork();
    if(cpid < 0){
      printf("fork failed inside namespace\n");
      exit(1);
    }

    if(cpid == 0){
      // Grandchild
      printf("4. Grandchild: getpid() = %d (should be 2)\n", getpid());
      exit(0);
    } else {
      wait(0);
    }

    exit(0);
  }

  // Parent
  wait(0);

  printf("5. Parent(after child exit): getpid() = %d (should be unchanged)\n",
         getpid());

  printf("--- PID Namespace Test Finished ---\n\n");

  exit(0);
}
