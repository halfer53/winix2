#include <sys/syscall.h>
#include <stdio.h>
#include <signal.h>
#include <sys/debug.h>
#include <stddef.h>
#include <errno.h>
#include <fs/const.h>
#include <sys/fcntl.h>

#define CHECK_SYSCALL(cond) \
if(!cond){ \
  failed_init(__LINE__); \
}

int failed_init(int line){
  printf("init failed at %d, err %d\n", line, errno);
  
  ___exit(1);
}

void init_init(){
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGSEGV);
  sigprocmask(SIG_SETMASK, &mask, NULL);
}

char shell_path[] = "/bin/shell";
char *shell_argv[] = {
  shell_path,
  NULL
};

int main(int argc, char **argv){
  struct message m;
  struct stat statbuf;
  pid_t pid;
  int status;
  int i, ret, fd, read_nr;

  ret = mkdir("/dev", 0x755);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/tty", 0x755, TTY_DEV);
  CHECK_SYSCALL(ret == 0);

  fd = open("/dev/tty", O_RDONLY, 0x755);
  CHECK_SYSCALL(fd == 0);

  ret = dup(fd);
  CHECK_SYSCALL(ret == 1);

  ret = dup(fd);
  CHECK_SYSCALL(ret == 1);

  pid = vfork();
  if(pid == 0){
    i = execv(shell_path, shell_argv);
  }

  init_init();
  
  while(1){
    pid = wait(&status);
    if(pid == -1){
      break;
    }
    // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }
  
  printf("Shutting down...\n");
  // winix_receive(&m);
  return 0;
}
