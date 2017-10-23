/**
 * 
 * Signal sending module
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:12:40
 * 
*/
#include <kernel/kernel.h>
#include <kernel/exception.h>
#include <winix/sigsend.h>
/**
    addui $sp, $sp, 1
    syscall
**/

/**
 * How does signal works in winix

 * When signal is to be delivered, it first checks if the signal is set to 
 * default or ignored. If it is, then the kernel choose the default action 
 * according to the signal. Currently, if handler is default, it simply kills the 
 * process. If it's ignored, $pc moves to the next instruction
 *
 * if neither of those two are set, the signal context will
 * be built on the user stack. The reason we do that is to simulate calling methods
 * for stack trace information.
 *
 * If a user signal action is set (by either signal(2) or sigaction(2)), the kernel
 * first saves process pcb info on to the user stack, which will be later restored 
 * after sig return sys call. Then the sigframe is built onto the stack after this.
 * The sigframe mainly consists code and messages for invoking sigreturn syscall.
 * The structure of the signal context look sth like below
 *
 * signum       <- top of the stack, popped by user signal handler
 * operation   
 * destination
 * pm           <- The above three are necessary parameters for invoking syscall
 *                 operation is WINIX_SEND, destination is SYSTEM, (or kernel)
 *                 and pm points to the messages being passed to the kernel. NB
 *                 that pm is the virtual memory
 * messages     <- The actual messages, remember that pm points to this message
 * PCB context  <- The previous pcb context saved, will be restored after sigreturn
 */

/**
 * Build signal context onto the user stack, PC points to the user
 * signal handler. Once building stack is successful, the next time 
 * the process runs, it will run the next process
 * @param  who    
 * @param  signum 
 * @return        OK if building is successful
 */
PRIVATE int build_signal_ctx(struct proc *who, int signum){
    struct sigframe sframe;
    struct sigframe* frame = &sframe;
    //pcb context are saved onto the user stack, and will be restored after sigreturn syscall
    copyto_user_stack(who,who,SIGNAL_CTX_LEN);

    frame->signum = signum;
    frame->s_base.operation = WINIX_SENDREC;
    frame->s_base.dest = SYSTEM_TASK;
    frame->s_base.pm = (struct message*)(who->sp - sizeof(struct message));
    frame->s_base.m.m1_i1 = signum;
    frame->s_base.m.type = SIGRET;

    //signum is sitting on top of the stack
    copyto_user_stack(who, frame, sizeof(struct sigframe));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = (reg_t*)who->sa_restorer;

    //backup sig mask
    who->sig_mask2 = who->sig_mask;
    who->sig_mask = who->sig_table[signum].sa_mask;

    who->flags |= IN_SIG_HANDLER;
    return OK;
}

/**
 * Check if the system can handle the signal (default, or ignore)
 * @param  who    
 * @param  signum 
 * @return        return OK if system has handled the signal
 *                return ERR if the user needs to handle the signal
 */
PRIVATE int sys_sig_handler(struct proc *who, int signum){
    if(who->sig_table[signum].sa_handler == SIG_DFL){

        switch(signum){
            case SIGCONT:
                if(who->state & STOPPING){
                    who->state &= ~STOPPING;
                    enqueue_schedule(who);
                }
                break;

            case SIGTSTP:
            case SIGSTOP:
                who->state |= STOPPING;
                who->sig_status = signum;
                check_waiting(who);
                break;
            
            default:
                KDEBUG(("Signal %d: kill process \"%s [%d]\"\n",signum,who->name,who->pid));
                exit_proc(who, 0, signum);
        }
        return OK;
    }
    //if it's ignored
    else if(who->sig_table[signum].sa_handler == SIG_IGN){
        struct proc* mp;
        KDEBUG(("Signal %d ignored by process \"%s [%d]\"\n",signum,who->name,who->pid));
        switch(signum){
            case SIGILL:
            case SIGABRT:
            case SIGFPE:
            case SIGSEGV:
                exit_proc(who, 0, signum);
                break;
                
            case SIGCHLD:
                foreach_child(mp, who){
                    if(mp->flags & ZOMBIE){
                        release_zombie(mp);
                        break;
                    }
                }
                break;

            default:
                break;
        }
        return OK;
    }
    return ERR;
}

int cause_sig(struct proc *who, int signum){
    struct sigaction* act;

    if(sigismember(&who->sig_mask, signum)){
        if(signum != SIGKILL && signum != SIGSTOP){
            int ret = 0;
            // kdebug("sig %d pending\n", signum);
            //if a signal is ignored and blocked by the process
            //it is quietly ignored, and not pended
            if(who->sig_table[signum].sa_handler != SIG_DFL){
                ret = sigaddset(&who->sig_pending, signum);
            }
            return ret;
        }
    }
    // kdebug("send sig %d to %d\n", signum, who->pid);

    //Unpause the process if it was blocked by pause(2)
    //or sigsuspend(2)
    if(who->state & PAUSING ){
        struct message m;
        // kdebug("wakeup %d\n", who->proc_nr);
        who->state &= ~PAUSING;
        syscall_reply(EINTR, who->proc_nr, &m);
    }

    //if the system can handle the signal
    if(sys_sig_handler(who,signum) == OK)
        return OK;

    act = &who->sig_table[signum];

    if(act->sa_flags & SA_NODEFER)
        sigaddset(&act->sa_mask, signum);
    else
        sigdelset(&act->sa_mask, signum);

    build_signal_ctx(who,signum);
    
    if(act->sa_flags & SA_RESETHAND){
        act->sa_handler = SIG_DFL;
    }
        
    return OK;
}


int check_sigpending(struct proc* who){
    int i;
    sigset_t* pendings = &who->sig_pending;
    sigset_t* blocked = &who->sig_mask;

    if(who->sig_pending){
        for(i = 1; i < _NSIG; i++){
            if(sigismember(pendings, i) && !sigismember(blocked, i)){
                sigdelset(pendings, i);
                cause_sig(who, i);
                return i;
            }
        }
    }
    return 0;
}

/**
 * send the signal, signal handler will be invoked next time the 
 * process is scheduled
 * @param  who    
 * @param  signum 
 * @return        
 */
int send_sig(struct proc *who, int signum){

    if(who->sig_table[signum].sa_handler != SIG_IGN &&
         is_in_syscall(who)){
        if(IS_SYSTEM(current_proc)){
            struct message m;
            syscall_reply(EINTR, who->proc_nr,&m);
        }else{
            get_proc(SYSTEM)->pc = &intr_syscall;
        }
    }

    cause_sig(who,signum);
    return OK;
}

