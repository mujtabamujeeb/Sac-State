/*
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * Kernel Interrupt Service Routines
 */

#include "spede.h"
#include "kernel.h"
#include "kisr.h"
#include "ksyscall.h"
#include "syscall.h"
#include "syscall_common.h"
#include "kutil.h"


/**
 * Kernel Interrupt Service Routine: Timer (IRQ 0)
 */
void kisr_timer() {

    /*
        {

          IO_DELAY();
          cons_printf("x",system_time);
        }
	
*/
	if(active_pid > 0){
		pcb[active_pid].active_time++;
		pcb[active_pid].total_time++;
	}
    // Increment the system time
    system_time++;

    // Dismiss IRQ 0 (Timer)
    outportb(0x20, 0x60);
}

void kisr_syscall(){
  int syscall;

    // if we do not have a valid pid, we should panic
    if(active_pid < 0 || active_pid > PID_MAX){
        panic("PANIC: DO NOT HAVE A VALID PID\n");
    }

    //syscall from the trapframe
    syscall = pcb[active_pid].trapframe_p->eax;

    switch (syscall){

      case SYSCALL_GET_SYS_TIME:
           ksyscall_get_sys_time();
          break;
      case SYSCALL_GET_PROC_PID:
           ksyscall_get_proc_pid();
          break;
      case SYSCALL_GET_PROC_NAME:
           ksyscall_get_proc_name();
          break;
      case SYSCALL_SLEEP:
           ksyscall_sleep();
          break;
      case SYSCALL_PROC_EXIT:
           ksyscall_proc_exit();
          break;
      case SYSCALL_SEM_INIT:
           ksyscall_sem_init();
          break;
      case SYSCALL_SEM_WAIT:
           ksyscall_sem_wait();
          break;
      case SYSCALL_SEM_POST:
           ksyscall_sem_post();
          break;
      case SYSCALL_MSG_SEND:
           ksyscall_msg_send();
          break;
      case SYSCALL_MSG_RECV:
           ksyscall_msg_recv();
          break;   

      default:
           panic("Invalid Syscall");
          break;
    }
}
