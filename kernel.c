/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * Main Kernel Entry point
 */

#include "spede.h"
#include "global.h"
#include "kernel.h"
#include "kutil.h"
#include "kproc.h"
#include "queue.h"
#include "kisr.h"
#include "user_proc.h"
#include "ipc.h"
#include "syscall.h"
#include "string.h"

/**
 * Kernel data structures - available to the entire kernel
 */

// Local function definitions
void kdata_init();

/**
 * Kernel data structures and variables
 * Externed via kernel.h
 */

// Current system time
int system_time;

// runtime stacks of processes
char stack[PROC_MAX][PROC_STACK_SIZE];

// process table
pcb_t pcb[PROC_MAX];

// System time
int system_time;

// ID of the actively running process, -1 means not set
int active_pid = -1;

// Process queues
queue_t available_q;
queue_t run_q;
queue_t idle_q;
queue_t sleep_q;
queue_t semaphore_q;
semaphore_t semaphores[SEMAPHORE_MAX];
mailbox_t mailboxes[MBOX_MAX];

/**
 * Kernel Initialization
 * Initializes all kernel data structures and variables
 */
void kernel_init() {
    size_t i = 0;
    int j=0;
    cons_printf("Initializing kernel data structures\n");

    // Initialize system time
	system_time = 0;

    //initializing the queues 
    printf("Initialization queue\n");
    queue_init(&available_q);
    printf("Initialization run queue\n");
    queue_init(&run_q);
    printf("Initialization sleep queue\n");
    queue_init(&sleep_q);
    printf("Initialization idle queue\n");
    queue_init(&idle_q);
    printf("Initialization semaphore queue\n");
    queue_init(&semaphore_q);

    for(i = 0; i<PROC_MAX;i++){
        semaphores[i].count = 0;
        semaphores[i].init =  SEMAPHORE_INITIALIZED;
        queue_init(&semaphores[i].wait_q);
        for(j = 0;j<MBOX_SIZE;j++){
            mailboxes[i].messages[j].sender = 0;
            mailboxes[i].messages[j].time_sent = 0;
            mailboxes[i].messages[j].time_received = 0;
            sp_memset(&mailboxes[i].messages[j].data,0,MSG_SIZE);
        } 
        mailboxes[i].head = 0;
        mailboxes[i].tail = 0;
        mailboxes[i].size = 0;
        queue_init(&mailboxes[i].wait_q);
        pcb[i].state =AVAILABLE;
        pcb[i].active_time = 0;
        pcb[i].total_time = 0;
        pcb[i].trapframe_p = 0;
        sp_memset(&pcb[i].name, 0,PROC_NAME_LEN);
        queue_in(&available_q, i);
        pcb[i].queue = &available_q;
    }

    //Feeding queues
    // queue_init(&available_q);
    // queue_init(&run_q);
    queue_in(&available_q,0);
    queue_in(&available_q,1);
    queue_in(&available_q,2);
    queue_in(&available_q,3);
    queue_in(&available_q,4);
    queue_in(&available_q,5);
    queue_in(&available_q,6);
    queue_in(&available_q,7);
    queue_in(&available_q,8);
    queue_in(&available_q,9);
    queue_in(&available_q,10);
    queue_in(&available_q,11);
    queue_in(&available_q,12);
}
/**
 * Kernel run loop
 *  - Process interrupts
 *  - Handle any "developer" commands
 *  - Run the process scheduler
 *
 * @param  trapframe - pointer to the current trapframe
 */
void kernel_run(trapframe_t *trapframe) {
    char key;

    // If we do not have a valid PID, then panic
    if (active_pid < 0 || active_pid > PID_MAX) {
        panic("Invalid PID!");
    }

    // save the trapframe into the PCB of the currently running process
    pcb[active_pid].trapframe_p = trapframe;

    // Process the current interrupt and call the appropriate service routine
    switch (trapframe->interrupt) {
        // Timer interrupt
        case TIMER_INTR:
            kisr_timer();
            break;
        
        case SYSCALL_INTR:
            kisr_syscall();
        break;

        default:
            panic("Invalid interrupt");
            break;
    }

    // Process special developer/debug commands
    if (cons_kbhit()) {
        key = cons_getchar();

        switch (key) {
            case 'b':
                // Set a breakpoint
                breakpoint();
                break;

            case 'n':
                // Create a new process
                kproc_exec("user_proc", &user_proc, &run_q);
                break;

            case 'p':
                // Trigger a panic (aborts)
                panic("User requested panic!");
                break;

            case 'x':
                // Exit the currently running process
                printf("Attempting to exit process %d\n", active_pid);
                kproc_exit(active_pid);
                break;

            case 'q':
                // Exit our kernel
                cons_printf("Exiting!!!\n");
                exit(0);
                break;

            default:
                // Display a warning (no abort)
                panic_warn("Unknown command entered");
                break;
        }
    }

    // Run the process scheduler
    kproc_schedule();
}
