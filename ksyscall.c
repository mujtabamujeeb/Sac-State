#include "spede.h"
#include "kernel.h"
#include "kproc.h"
#include "kutil.h"
#include "string.h"
#include "queue.h"
#include "ksyscall.h"

int mbox_enqueue(msg_t *msg, int mbox_num);
int mbox_dequeue(msg_t *msg, int mbox_num);

/**
 * System call kernel handler: get_sys_time
 * Returns the current system time (in seconds)
 */
void ksyscall_get_sys_time() {

    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }  
    // Copy the system time from the kernel to the
    // eax register via the running process' trapframe
    pcb[active_pid].trapframe_p -> ebx = system_time/100;
}

/**
 * System call kernel handler: get_proc_id
 * Returns the currently running process ID
 */
void ksyscall_get_proc_pid() {
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }

    // Copy the running pid from the kernel to the
    // eax register via the running process' trapframe
    pcb[active_pid].trapframe_p->ebx=active_pid;
}

/**
 * System call kernel handler: get_proc_name
 * Returns the currently running process name
 */
void ksyscall_get_proc_name() {
    char *dest;
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }

    // Set the pointer to the address passed in via EBX

    // Copy the string name from the PCB to the destination
    dest = (char *)pcb[active_pid].trapframe_p -> ebx;
    sp_memcpy(dest, &pcb[active_pid].name, sp_strlen(pcb[active_pid].name));

    // Set the return code
    
}

/**
 * System call kernel handler: sleep
 * Puts the currently running process to sleep
 */
void ksyscall_sleep() {
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }
    // Calculate the wake time for the currently running process
    pcb[active_pid].wake_time = pcb[active_pid].trapframe_p->eax*100 + system_time;

    // Move the currently running process to the sleep queue
    queue_in(&sleep_q,active_pid);
    // Change the running process state to SLEEPING
    pcb[active_pid].state = SLEEPING;

    // Clear the running PID so the process scheduler will run
    active_pid = -1;
}

void ksyscall_proc_exit() {
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }
    // Trigger the process to exit
    kproc_exit(active_pid);
}

// The "semaphore" passed in is a pointer to a variable that will contain the semaphore id. By default, this variable should be set to "SEMAPHORE_UNINITIALIZED" (-1).
// If a call to sem_init() is called on a semaphore that is not already initialized (i.e. the semaphore id is set to SEMAPHORE_UNINITIALIZED), it should:
// Check if the semaphore has indicated it is initialized in the kernel (via the semaphore init value). If not, you should:
// Queue out a semaphore from the semaphore queue
// Set the semaphore id to the item that was queued out
// Ensure that the semaphore count is set to 0
// Ensure that the semaphore init flag is set to SEMAPHORE_INITIALIZED
// If a call to sem_init() is called on a semaphore that is already initialized (i.e. the semaphore id is NOT SEMAPHORE_UNINITIALIZED) and the semaphore init flag is set to SEMAPHORE_INITIALIZED it should:
// Obtain the semaphore id from the pointer passed in
// Ensure that the semaphore count is initialized to 0

void ksyscall_sem_init(){
    //declaring semaphore
    sem_t *sem;
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }

    sem = (sem_t *)pcb[active_pid].trapframe_p->ebx;
    if(*sem == SEMAPHORE_UNINITIALIZED){
        //index to an arraay of semaphore and checking inti not equal to intialize so equal to -1
        if(semaphores[*sem].init != SEMAPHORE_INITIALIZED){
            queue_out(&semaphore_q, sem);
            semaphores[*sem].count = 0;
            semaphores[*sem].init = SEMAPHORE_INITIALIZED;
            queue_init(&semaphores[*sem].wait_q);
        }
    }
    else if (semaphores[*sem].init == SEMAPHORE_INITIALIZED){
        semaphores[*sem].count = 0;
    }

}

// Waits on a semaphore to be posted.
// For the passed in semaphore, determine if the semaphore id is valid. If it is not valid, panic.
// If the semaphore count is > 0, then it means that at least one process is already waiting. In this case, the process should be unscheduled and moved into the wait queue for the given semaphore. The process state should be WAITING in this case.
// The semaphore count should be incremented whenever a call to sem_wait is performed.
void ksyscall_sem_wait(){
    sem_t *sem;
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }

    sem = (sem_t *)pcb[active_pid].trapframe_p->ebx;
    if(semaphores[*sem].count > 0 && pcb[active_pid].state == ACTIVE){
        queue_out(&semaphore_q, sem);
        queue_in(&semaphores[*sem].wait_q, *sem);
        pcb[active_pid].state = WAITING;
    }
    semaphores[*sem].count++;
    
}
// Posts a semaphore and releases the first process that was waiting on the semaphore.
// For the passed in semaphore, determine if the semaphore id is valid. If it is not valid, panic.
// If the semaphore has a process that is waiting, move the process from the semaphore wait queue to the kernel run queue. Ensure that when this happens, the process state is set to RUNNING.
// If the semaphore count is > 0, it should be decremented.

void ksyscall_sem_post(){
    
    sem_t *sem;
    
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }
    sem = (sem_t *)pcb[active_pid].trapframe_p->ebx;
    if(semaphores[*sem].count > 0 && pcb[active_pid].state == ACTIVE){
        queue_out(&semaphore_q, sem);
        queue_in(&run_q, *sem);
        pcb[active_pid].state = RUNNING;
        semaphores[*sem].count--;
    }
    
    
}

// Sends a message to the specified mailbox. This is a non-blocking operation. The calling process will proceed once the message is "sent" to the mailbox.
// A message is sent by queuing it into the specified mailbox.
// If the mailbox is full (message queue is full) the system call handler should panic.
// If the mailbox has a process in it's wait queue, it should:
// Queue out from the mailbox wait queue and move the process to the kernel run queue
// Ensure that the state is set to RUNNING
// Obtain the message pointer from the receiving process' trapframe
// Queue out the message from the mailbox to the receiving process

void ksyscall_msg_send(){
    msg_t *msg;
    int mbox_num;
    int pid;
    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }

    //from the trapframe 
    msg = (msg_t *)pcb[active_pid].trapframe_p->ebx;
    mbox_num = pcb[active_pid].trapframe_p->ecx;

    //mailbox if full
    if(mailboxes[mbox_num].size == MBOX_SIZE){
        panic("message box is at its capacity");
    }

    if(mailboxes[mbox_num].wait_q.size > 0){
        queue_out(&(mailboxes[mbox_num].wait_q), &pid);
        queue_in(&run_q, pid);
        pcb[pid].state = RUNNING;
        sp_memcpy(((msg_t *)pcb[pid].trapframe_p->ebx), &msg, sizeof(msg));
    }
    else{
        mbox_enqueue(msg, mbox_num);
    }
    
}

// Receives a message from the specified mailbox. This is a blocking operation - if the mailbox is empty, the process will not proceed - it should wait. If the mailbox has a message, it can be "received" immediately and the calling process can proceed.
// If the mailbox has a message
// Dequeue it to the message pointer via the running process' trapframe
// Panic if a message cannot be dequeued
// If there is no message in the mailbox
// Move the process to the specified mailbox wait queue
// Set the state to WAITING
// Clear the run pid so another process can be scheduled
void ksyscall_msg_recv(){
    msg_t *msg;
    int mbox_num;

    // Don't do anything if the running PID is invalid
    if (active_pid < 0 || active_pid > PID_MAX) {
        //not doing anything
    }
    msg = (msg_t *)pcb[active_pid].trapframe_p->ebx;
    mbox_num = pcb[active_pid].trapframe_p->ecx;

    if(mailboxes[mbox_num].size == 0){
        queue_in(&(mailboxes[mbox_num].wait_q), active_pid);
        pcb[active_pid].state = WAITING;
        active_pid = -1;
    }
    else{
        mbox_dequeue(msg, mbox_num);
    }

}


// The mailbox enqueue function will behave similar to your normal queue, except that it will use an array of messages versus an array of integers for the items within your queue.
// When enqueueing an item, you should copy the message to the specified mailbox message using the source message pointer.
int mbox_enqueue(msg_t *msg, int mbox_num){

    sp_memcpy(&(mailboxes[mbox_num].messages[mailboxes[mbox_num].tail]), &msg, sizeof(msg));

    mailboxes[mbox_num].tail++;

    if(mailboxes[mbox_num].tail == MBOX_SIZE){
        mailboxes[mbox_num].tail = 0;
    }

    mailboxes[mbox_num].size++;
    return 0;

}

// The mailbox enqueue function will behave similar to your normal queue, except that it will use an array of messages versus an array of integers for the items within your queue.
// When dequeuing an item, you should copy the message from the specified mailbox message using the destination message pointer.
int mbox_dequeue(msg_t *msg, int mbox_num){
    
    sp_memcpy(&msg, &(mailboxes[mbox_num].messages[mailboxes[mbox_num].head]), sizeof(mailboxes[mbox_num].messages[mailboxes[mbox_num].head]));

    //moving the head
    mailboxes[mbox_num].head++;

    if(mailboxes[mbox_num].head == MBOX_SIZE){
        mailboxes[mbox_num].head = 0;
    }

    mailboxes[mbox_num].size--;

    return 0;
}

int mbox_full(int mbox_num){
    if(mailboxes[mbox_num].size == MBOX_SIZE){
        return 1;
    }
    return 0;
}

int mbox_empty(int mbox_num){
    if(mailboxes[mbox_num].size == 0){
        return 1;
    }
    return 0;
}