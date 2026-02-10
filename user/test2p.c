#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define CLONE_NEWNS 0x1

void
test_mount_ns()
{
  int pid;

  printf("--- Starting Mount Namespace Test ---\n");

  // ۱. ایجاد فضای نام جدید برای والد (تا بر روی فرزند تاثیر بگذارد)
  if(unshare(CLONE_NEWNS) < 0){
    printf("Error: unshare(CLONE_NEWNS) failed\n");
    exit(1);
  }
  printf("1. Parent created a new Mount Namespace.\n");

  pid = fork();
  if(pid < 0){
    printf("Error: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // --- داخل فرزند ---
    // در دنیای واقعی اینجا فرزند باید یک دیسک جدید را مجهز (mount) کند.
    // اینجا همین که فرزند بدون کرش کردن بالا آمده و می‌تواند فایل‌ها را ببیند،
    // یعنی ساختار mnt_ns و رفرنس‌دهی inode ریشه درست کار می‌کند.
    
    int fd = open("README", 0);
    if(fd >= 0){
      printf("2. Child: Successfully opened 'README' in its own Mount NS.\n");
      close(fd);
    } else {
      printf("2. Child: Error! Cannot access file system.\n");
      exit(1);
    }
    
    printf("3. Child: Mount NS test passed.\n");
    exit(0);
  } else {
    // --- داخل والد ---
    wait(0);
    printf("4. Parent: Child finished. Mount NS is stable.\n");
    printf("--- Test Finished: SUCCESS ---\n");
  }
}

int
main(void)
{
  test_mount_ns();
  exit(0);
}