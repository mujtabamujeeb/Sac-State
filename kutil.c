/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * Internal Kernel APIs
 */
#include "spede.h"

/**
 * Triggers a kernel panic that does the following:
 *   - Displays a panic message on the target console
 *   - Triggers a breakpiont (if running through GDB)
 *   - aborts/exits
 * @param msg   the message to display
 */
void panic(char *msg) {
    // Display a message indicating a panic was hit
    printf(msg);
    // trigger a breakpoint
    breakpoint();
    // abort since this is a fatal condition!
    abort();
}

/**
 * Triggers a kernel panic that does the following:
 *   - Displays a warning message on the target console
 *   - Triggers a breakpoint (if running through GDB)
 * @param msg   the message to display
 */
void panic_warn(char *msg) {
    // Display a message indicating a warning was hit
    printf(msg);
    // trigger a breakpoint
    breakpoint();
}
