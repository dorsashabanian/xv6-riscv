#include "kernel/types.h"
#include "user/user.h"
#include "kernel/syscall.h"

void
test_trace() {
  int pid;

  printf("=== TRACE TEST START ===\n");

  // ردیابی سیستم‌کال write (بیت مربوط به SYS_write را روشن می‌کنیم)
  trace(1 << SYS_write);

  printf("A\n"); // خروجی کرنل: syscall write -> 2
  printf("B\n"); // خروجی کرنل: syscall write -> 2

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // فرزند باید ماسک را از والد ارث برده باشد
    printf("child\n"); // خروجی کرنل: syscall write -> 6
    
    // تست سیستم‌کال exec (ماسک exec را هم اضافه می‌کنیم)
    trace((1 << SYS_write) | (1 << SYS_exec));
    
    char *args[] = {"echo", "exec-ok", 0};
    exec("echo", args);
    printf("exec failed\n");
    exit(1);
  } else {
    wait(0);
    printf("parent\n"); // خروجی کرنل: syscall write -> 7
    
    // خاموش کردن ترِیس
    trace(0);
    printf("NO TRACE\n"); // هیچ لاگی نباید چاپ شود
  }

  printf("=== TRACE TEST END ===\n");
}

int
main(void)
{
  test_trace();
  exit(0);
}