#include <lib.h>

int kill (pid_t pid, int sig){
	struct message m;
	m.i1 = pid; 
	m.i2 = sig;
	return _SYSCALL(SYSCALL_KILL,&m);
}
