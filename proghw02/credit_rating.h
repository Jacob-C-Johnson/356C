/*
 * credit_rating.h - Struct and function prototypes for credit rating program
 *
 * Author: Jacob Johnson
 * Date: 10/09/2024
 *
 * Assignment: HW-Prog02
 * Course: CSCI 356
 * Version 1.0
 */
#ifndef CREDIT_RATING_H_
#define CREDIT_RATING_H_
#include "my_queue.h"


// Structure for holding credit rating data
typedef struct {
    char name[50];
    int creditRating;
} CreditRating;

/*
 * Function to print a single credit rating
 * CreditRating* person: a pointer to a CreditRating struct to print
 * int maxRating: the maximum credit rating in the queue
 */
void PrintCreditRating(CreditRating* person, int maxRating);

/*
 * Iterates through the queue and finds the maximum credit rating
 * queue q: a queue to check for emptiness; q must not be NULL
 * returns: returns the max credit rating in the queue if it is not null. In null case, returns 0
 */
int GetMaxRating(queue q);

/*
 * iterates through the given queue and processes each credit rating then prints the results
 * queue q: 	a queue to append; q must not be NULL
 * void* item:	a pointer to an item to be enqueued onto queue
 */
void ProcessCreditRating(queue q);

#endif /* CREDIT_RATING_H_ */
