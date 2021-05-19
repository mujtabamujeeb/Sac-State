/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * Kernel Process Handling
 */
#include "spede.h"
#include "kernel.h"
#include "kutil.h"
#include "kproc.h"
#include "queue.h"
#include "string.h"

/**
 * Process scheduler
 */
void kproc_schedule() {
	
    // Once the active process has exceeded the maximum
    // number of ticks, it needs to be unscheduled:
    // reset the current active time
    int wakeup_pid;
    int sleep_pid;

    while(sleep_q.size != 0){

        sleep_pid = sleep_q.items[sleep_q.head];
        wakeup_pid = pcb[sleep_pid].wake_time;

        if(wakeup_pid <= system_time){

            pcb[sleep_pid].state = RUNNING;
            queue_out(&sleep_q, &sleep_pid);
            
            if(sleep_pid == 0){

                pcb[sleep_pid].queue = &idle_q;
                queue_in(&idle_q, sleep_pid);
            }
            else{
                
                pcb[sleep_pid].queue = &run_q;
                queue_in(&run_q, sleep_pid);
            }
        }
        else{
            break;
        }
    }

    if(active_pid > -1){
    //   reset the current active time
        pcb[active_pid].active_time = 0;
    //   set the state to RUNNING
        pcb[active_pid].state = RUNNING; 
    //   queue the process back into the running queue
        if(active_pid == 0){
            pcb[active_pid].queue = &idle_q;
            queue_in(&idle_q, active_pid); 
        }else{
            pcb[active_pid].queue = &run_q;
            queue_in(&run_q, active_pid);
        }
    //   clear the active pid
        active_pid = -1;

    }

    // if we don't have an actively running process:
    if (active_pid == -1) {
        if(run_q.size == 0){
            //queue_out the process from the running queue and set it to the active pid
            queue_out(&idle_q, &active_pid);
        }else{
            //queue_out the process from the running queue and set it to the active pid
            queue_out(&run_q, &active_pid);
        }
        if(active_pid >= 0 && active_pid <= PID_MAX) {
        //   set the state in the process control block for the new active proces to ACTIVE
            pcb[active_pid].state = ACTIVE;
        }
    }


    // if we do not have a valid pid, we should panic
    if(active_pid < 0 || active_pid > PID_MAX){
        panic("PANIC: DO NOT HAVE A VALID PID\n");
    }

    // Load the next process
    kproc_load(pcb[active_pid].trapframe_p);
}

/**
 * Start a new process
 * @param proc_name The process title
 * @param proc_ptr  function pointer for the process
 * @param queue     the run queue in which this process belongs
 */
void kproc_exec(char *proc_name, void *proc_ptr, queue_t *queue) {
    int pid; 
    int i=0;

    // Ensure that valid parameters have been specified and panic otherwise
	if(!proc_name||!proc_ptr||!queue){
    	panic("Prameters not specified\n");
	} 
	pid = (int)proc_ptr;

    // Dequeue the process from the available queue
    if (queue_out(&available_q, &pid) != 0) {
        panic_warn("Unable to retrieve process from unused queue\n");
        return;
    }

    // Initialize the PCB entry for the process (e.g. pcb[pid])
    // Set the process state to RUNNING
    pcb[pid].state = RUNNING;
	pcb[pid].queue = NULL;
	pcb[pid].trapframe_p = NULL;
    // Initialize other process control block variables to default values
    pcb[pid].active_time = 0; //default value set to 0 for active
    pcb[pid].total_time = 0; //default value set to 0 for total_time
    // Copy the process name to the PCB
    sp_strcpy(pcb[pid].name, proc_name);
    
    // Ensure the stack for the process is cleared (e.g. stack[pid])
    while(i<PROC_STACK_SIZE){
        stack[pid][i]=-1;
        i++;
    }

    // Allocate the trapframe data
    pcb[pid].trapframe_p = (trapframe_t *)&stack[pid][PROC_STACK_SIZE - sizeof(trapframe_t)];

    // Set the instruction pointer in the trapframe
    pcb[pid].trapframe_p->eip = (unsigned int)proc_ptr;

    // Set INTR flag
    pcb[pid].trapframe_p->eflags = EF_DEFAULT_VALUE | EF_INTR;

    // Set each segment in the trapframe
    pcb[pid].trapframe_p->cs = get_cs();
    pcb[pid].trapframe_p->ds = get_ds();
    pcb[pid].trapframe_p->es = get_es();
    pcb[pid].trapframe_p->fs = get_fs();
    pcb[pid].trapframe_p->gs = get_gs();

    // Set the process run queue
    pcb[pid].queue = queue;

    // Move the proces into the associated run queue
    queue_in(pcb[pid].queue, pid);

    printf("Executed process %s (%d)\n", pcb[pid].name, pid);
}

/**
 * Exit the currently running process
 */
void kproc_exit(int pid) {
    // PID 0 should be our kernel idle task. It should never exit.
    if(pid == 0){
    // In this case, print a message to the host terminal
        cons_printf("Kernel at idling task\n");
    }
    // Panic if we have an invalid PID
    if(pid < 0){
        panic("We have an invalid PID\n");
    }

    // Panic warn if the PID is not running/active; and return
    //if the state is not running basically, warn and just return;
    if(pcb[pid].state == AVAILABLE){
        panic_warn("PID is not running/active\n");
        return;
    }
    
    // Clear the PCB for the process and set the process state to AVAILABLE
    pcb[pid].total_time = 0;//cleared total time
    pcb[pid].total_time = 0;//cleared active time
    pcb[pid].state = AVAILABLE; //prcoess state set to AVAILABLE

    // Queue the pid back to the available queue
    queue_in(&available_q,pid);

    // if the pid is the active pid, then clear the active pid and
    if(pid == active_pid){
    // trigger the the process scheduler to schedule a new process
        active_pid = 0; // clearing the active PID
        kproc_schedule();//process scheduler is trigered for a new process
    }
    //Done!!
}


/**
 * Kernel idle task
 */
void ktask_idle() {
    int i;

    // Indicate that the Idle Task has started
    cons_printf("idle_task started\n");

    // Process run loop
    while (1) {
        cons_printf("idle_task running\n");

        // busy loop/delay
        for (i = 0; i < IO_DELAY_LOOP; i++) {
            IO_DELAY();
        }
    }
}
