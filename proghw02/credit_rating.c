/*
 * credit_rating.c - Program to manage and process credit ratings using a queue
 *
 * Author: Jacob Johnson
 *
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
int get_max_rating(queue q) {
    int max_rating = 0;
    q_element* current = q->front;

    while (current != NULL) {
        CreditRating* person = (CreditRating*)current->contents;
        if (person->creditRating > max_rating) {
            max_rating = person->creditRating;
        }
        current = current->next;
    }

    return max_rating;
}

// Function to print a single credit rating
void print_credit_rating(CreditRating* person, int max_rating) {
    int distance = max_rating - person->creditRating;
    printf("%s: %d, Distance: %d\n", person->name, person->creditRating, distance);
}

// Function to process credit ratings (dequeue and calculate averages)
void process_credit_ratings(queue q) {
    int total = 0;
    int count = 0;
    int max_rating = get_max_rating(q);

    printf("Name   Rating    Distance\n");

    while (!isempty(q)) {
        CreditRating* person = (CreditRating*)dequeue(q);
        print_credit_rating(person, max_rating);
        total += person->creditRating;
        count++;
        free(person);  // Free the memory of each person
    }

    if (count > 0) {
        printf("Average: %d\n", total / count);
    }
}

// Main function to drive the program
int main() {
    queue q = newqueue();
    char input[MAX_NAME_LENGTH];
    int rating;

    printf("Enter credit rating information (Name Rating). Press enter on a blank line to finish:\n");

    while (1) {
        printf("Enter name and rating: ");
        fgets(input, sizeof(input), stdin);

        // Check for blank line to terminate input
        if (strcmp(input, "\n") == 0) {
            break;
        }

        // Parse input
        char name[MAX_NAME_LENGTH];
        sscanf(input, "%s %d", name, &rating);

        // Create a new person and enqueue it
        CreditRating* person = (CreditRating*)malloc(sizeof(CreditRating));
        strcpy(person->name, name);
        person->creditRating = rating;

        enqueue(q, person);
    }

    // Process the ratings and output results
    process_credit_ratings(q);

    return 0;
}