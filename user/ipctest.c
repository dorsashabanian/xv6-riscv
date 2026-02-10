
#include "kernel/types.h"
#include "user/user.h"

#define CLONE_NEWIPC 0x8

int main() {
  printf("--- Starting IPC Namespace Test ---\n");

  if(unshare(CLONE_NEWIPC) < 0) {
    printf("IPC Unshare Failed!\n");
    exit(1);
  }

  int pid = fork();
  if(pid == 0) {
    printf("Child: Running in isolated IPC namespace.\n");
    exit(0);
  } else {
    wait(0);
    printf("Parent: IPC namespace test passed.\n");
  }

  printf("--- Test Finished: SUCCESS ---\n");
  exit(0);
}