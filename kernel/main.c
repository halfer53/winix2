/**
 * 
 * Main Entry point for Winix
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * @modify date 2017-08-23 05:59:52
*/

#include "winix.h"
#include <winix/rex.h>
#include <init_codes.c>
#include <shell_codes.c>


/**
 * Entry point for WINIX.
 **/
void main() {

	struct proc *p = NULL;
	size_t *ptr = NULL;
	void *addr_p = NULL;
	int proc_nr = 0;
	int i=0;

	//Print boot message.

	init_bitmap();
	init_mem_table();
	init_proc();

	//Initialise the system task
	p = start_kernel_proc(system_main, SYSTEM_PRIORITY, "SYSTEM");
	ASSERT(p != NULL);

	p = start_user_proc(init_code,2,0, USER_PRIORITY,"init");
	p->quantum = 1;

	p = start_user_proc(shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	ASSERT(p != NULL);
	p->parent = 1;//hack 

	init_slab(shell_code,shell_code_length);
	
	//Initialise exceptions
	init_exceptions();
	//Kick off first task. Note: never returns
	sched();
}
