#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

int
main(void)
{
  char buf[64];
  int pid;

  printf("\n--- Starting Integrated Namespace Test ---\n");

  // Parent before fork
  gethostname(buf,64);
  printf("1. Parent: Original Hostname: %s, Real PID: %d\n", buf, getpid());

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child
    printf("2. Child: calling unshare(CLONE_NEWUTS)\n");
    unshare(CLONE_NEWUTS);

    sethostname("isolated-node",13);

    gethostname(buf,64);
    printf("3. Child: My Isolated Hostname is: %s\n", buf);

    exit(0);
  }

  // Parent
  wait(0);

  gethostname(buf,64);
  printf("4. Parent: My Hostname is still: %s (Should be xv6)\n", buf);
  printf("5. Parent: My PID is still: %d\n", getpid());

  printf("--- Test Finished ---\n\n");

  exit(0);
}
