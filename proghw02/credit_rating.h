/*
 * credit_rating.h - Struct and function prototypes for credit rating program
 *
 * Author: Jacob Johnson
 *
 * Course: CSCI 356
 * Version 1.0
 */

#ifndef CREDIT_RATING_H_
#define CREDIT_RATING_H_

// Structure for holding credit rating data
typedef struct {
    char name[50];
    int creditRating;
} CreditRating;

// Function prototypes
void print_credit_rating(CreditRating* person, int max_rating);
int get_max_rating(queue q);
void process_credit_ratings(queue q);

#endif /* CREDIT_RATING_H_ */
