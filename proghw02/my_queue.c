/*
 * my_queue.c - Implementation of queue functions
 *
 * Author: clintf
 * Student Name: Jacob Johnson
 *
 * Course: CSCI 356
 * Version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "my_queue.h"

// Creates and returns a new queue
queue newqueue() {
    queue q = (queue)malloc(sizeof(struct queueS));
    if (q == NULL) {
        fprintf(stderr, "Failed to create a new queue\n");
        exit(1);
    }
    q->front = NULL;
    return q;
}

// Checks if the queue is empty
int isempty(const queue q) {
    return q->front == NULL;
}

// Adds an item to the end of the queue
void enqueue(queue q, void* item) {
    q_element* new_elem = (q_element*)malloc(sizeof(q_element));
    if (new_elem == NULL) {
        fprintf(stderr, "Failed to allocate memory for a new queue element\n");
        exit(1);
    }
    new_elem->contents = item;
    new_elem->next = NULL;

    if (isempty(q)) {
        q->front = new_elem;
    } else {
        q_element* current = q->front;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_elem;
    }
}

// Removes and returns the first item from the queue
void* dequeue(queue q) {
    if (isempty(q)) {
        return NULL;
    }

    q_element* front_elem = q->front;
    void* item = front_elem->contents;
    q->front = front_elem->next;

    free(front_elem);
    return item;
}

// Returns the first item without removing it
void* peek(queue q) {
    if (isempty(q)) {
        return NULL;
    }
    return q->front->contents;
}
