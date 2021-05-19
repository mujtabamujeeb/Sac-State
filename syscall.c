#include "syscall.h"
#include "syscall_common.h"
#include "kernel.h"
#include "spede.h"

/*
 * Anatomy of a system call
 *
 * Data to/from the kernel is exchanged via the CPU trapframe
 * The system call is triggered via an interrupt that the kernel processes
 * Data can be sent to and received from the kernel in a single system call
 *
 * Data from the kernel:
 * int MySyscall() {
 *     int x;
 *     asm("movl %1, %%eax;"    // eax register indicates the syscall
 *         "int $0x80;"           // trigger the syscall
 *         "movl %%ebx, %0;"    // pull data back from the kernel via register
 *         : "=g" (x)           // operand 0 is data returned from the kernel
 *         : "g" (SYSCALL_FOO)  // operand 1 is the syscall
 *         : "eax", "ebx");     // restore the registers that were used
 *     return x;
 * }
 *
 * Data to the kernel:
 * void MySyscall(int x) {
 *     int x;
 *     asm("movl %0, %%eax;"    // eax register indicates the syscall
 *         "movl %1, %%ebx;"    // send data to the kernel
 *         "int $0x80;"           // trigger the syscall
 *         :                    // no operands for return data
 *         : "g" (SYSCALL_FOO), // operand 0 is the syscall
 *           "g" (x)            // operand 1 is the data we are sending
 *         : "eax", "ebx");     // restore the registers that were used
 * }
 *
 * Data to and from the kernel:
 * int MySyscall(int x) {
 *     int y;
 *     asm("movl %1, %%eax;"    // eax register indicates the syscall
 *         "movl %2, %%ebx;"    // send data to the kernel
 *         "int $0x80;"           // trigger the syscall
 *         "movl %%ebx, %3;"    // pull data back from the kernel via register
 *         : "=g" (y)           // operand 0 is data returned from the kernel
 *         : "g" (SYSCALL_FOO), // operand 1 is the syscall
 *           "g" (x)            // operand 2 is the data we are sending
 *         : "eax", "ebx");     // restore the registers that were used
 *     return y;
 * }
 */

void proc_exit() {

        asm("movl %0, %%eax;" 
        "int $0x80;"
        :
        : "g" (SYSCALL_PROC_EXIT)
        : "eax");

}

int get_sys_time() {
    int time = -1;

        asm("movl %1, %%eax;"
            "int $0x80;"
            "movl %%ebx, %0;"
            : "=g"(time)
            : "g"(SYSCALL_GET_SYS_TIME)
            : "eax", "ebx");

    return time;
}

int get_proc_pid() {

    int pid = -1;
        asm("movl %1, %%eax;"
            "int $0x80;"
            "movl %%ebx, %0;" 
            : "=g"(pid)
            : "g"(SYSCALL_GET_PROC_PID)
            : "eax", "ebx");
    return pid;
}

int get_proc_name(char *name) {
    int rc = -1;
    if(name != '\0'){

    asm("movl %1, %%eax;"
        "movl %0, %%ebx;"
        "int $0x80;"
        : "=g"(name)
        : "g"(SYSCALL_GET_PROC_NAME)
        : "eax", "ebx");
    
        if(name != '\0'){
            rc = 0;
        }
    }
    return rc;

}

void sleep(int seconds) {

    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_SLEEP),
          "g" (seconds)
        : "eax", "ebx");

}

void sem_init(sem_t *sem){
    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_SEM_INIT),
          "g"(sem)
        : "eax", "ebx");

}

void sem_wait(sem_t *sem){
    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_SEM_WAIT),
          "g"(sem)
        : "eax", "ebx");

}

void sem_post(sem_t *sem){
    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_SEM_WAIT),
          "g"(sem)
        : "eax", "ebx");

}

void msg_send(msg_t *msg, int mbox_num){
    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_MSG_SEND),
          "g"(msg),"g"(mbox_num)
        : "eax", "ebx", "ecx");
}

void msg_recv(msg_t *msg, int mbox_num){
    asm("movl %0, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "int $0x80;"
        :
        : "g"(SYSCALL_MSG_RECV),
          "g"(msg),"g"(mbox_num)
        : "eax", "ebx", "ecx");
}



