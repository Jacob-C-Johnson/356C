/*
 * credit_rating.c - Program to manage and process credit ratings using a queue
 *
 * Author: Jacob Johnson
 * Date: 10/09/2024 
 *
 * Assignment: HW-Prog02
 * Course: CSCI 356
 * Version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "credit_rating.h"
#include "my_queue.h"

#define MAX_NAME_LENGTH 50

// Function to calculate max credit rating
int GetMaxRating(queue q) {
    int maxRating = 0;
    q_element* current = q->front;

    // Iterate through the queue and find the maximum credit rating
    while (current != NULL) {
        CreditRating* person = (CreditRating*)current->contents; 
        if (person->creditRating > maxRating) {
            maxRating = person->creditRating;
        }
        current = current->next;
    }

    return maxRating;
}

// Function to print a single credit rating
void PrintCreditRating(CreditRating* person, int maxRating) {
    int distance = maxRating - person->creditRating;
    printf("%s\t %d\t %d\n", person->name, person->creditRating, distance);
}

// Function to process credit ratings (dequeue and calculate averages)
void ProcessCreditRating(queue q) {
    int total = 0;
    int count = 0;
    int maxRating = GetMaxRating(q);

    printf("Name   Rating    Distance\n");

    // While the queue is not empty, dequeue and process each person
    while (!isempty(q)) {
        CreditRating* person = (CreditRating*)dequeue(q);
        PrintCreditRating(person, maxRating);
        total += person->creditRating;
        count++;
        free(person);  // Free the memory of each person
    }

    if (count > 0) {
        printf("\nAverage Credit Rating: %d\n", total / count);
        printf("Highest Credit Rating: %d\n", maxRating);
    }
}

// Main function to drive the program
int main() {
    // Set up the queue and input variables
    queue q = newqueue();
    char input[MAX_NAME_LENGTH];
    int rating;

    // Explain the input and prompt the user
    printf("\nEnter credit rating information seperated by a space (Name Rating). Press enter on a blank line to finish.\n");

    // Loop to read input and enqueue it
    while (1) {
        // Get input
        printf("Enter name and rating: ");
        fgets(input, sizeof(input), stdin);

        // Check for blank line to terminate input
        if (strcmp(input, "\n") == 0) {
            printf("\nProcessing credit ratings...\n\n");
            break;
        }

        // Parse input with sscanf to seperate name and rating
        char name[MAX_NAME_LENGTH];
        sscanf(input, "%s %d", name, &rating);

        // Create a new person and enqueue it
        CreditRating* person = (CreditRating*)malloc(sizeof(CreditRating));
        strcpy(person->name, name);
        person->creditRating = rating;

        enqueue(q, person);
    }

    // Process the ratings and output results
    ProcessCreditRating(q);

    return 0;
}
