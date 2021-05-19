/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * Spring 2021
 *
 * Queue Utilities
 */

#include "queue.h"
#include "spede.h"

/**
 * Initializes an empty queue
 * Sets the empty queue items to -1
 *
 * @param  queue - pointer to the queue
 * @return -1 on error; 0 on success
 */
int queue_init(queue_t *queue) {

    size_t i;	

    if(queue == NULL){
        printf("QUEUE is empty!\n");
        return -1;
    }

    // Populate all queue items with the QUEUE_EMPTY_VALUE
	for(i = 0; i < QUEUE_SIZE; i++){

		queue->items[i] = QUEUE_EMPTY_VALUE; 
	}	
    // Initialize all queue member values to default
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;

    return 0;
}

/**
 * Adds an item in to the tail of a queue
 * @param  queue - pointer to the queue
 * @param  item  - the item to add
 * @return -1 on error; 0 on success
 */
int queue_in(queue_t *queue, int item) {
    // Return an error if the queue is full
    if (queue->size == QUEUE_SIZE) {
		return -1;
    }

    queue->items[queue->tail] = item; // Add the item to the tail of the queue
    queue->tail=queue->tail +1;       // Move the tail to beginning

    if(queue->tail == QUEUE_SIZE){

        queue->tail = 0;

    }

    queue->size = queue->size + 1;

    return 0;
}

/**
 * Pulls an item from out from the head of the specified queue
 * @param  queue - pointer to the queue
 * @return -1 on error; 0 on success
 */
int queue_out(queue_t *queue, int *item) {

	if (queue == NULL){
        // return -1 if queue is empty
        return -1;
    }
    
        // Get the item from the head of the queue
        *item = queue->items[queue->head];
        // Reset the empty item
		queue->items[queue->head] = QUEUE_EMPTY_VALUE;
        // Move the head forward
        queue->head = queue->head + 1;
        // If we are at the end of the array, move the head to the beginning
        if(queue->head == QUEUE_SIZE) { 
            queue->head = 0;
            
        }
        // Decrement size (since we just removed an item from the queue)
        queue->size--;    
    
    return 0;
}
