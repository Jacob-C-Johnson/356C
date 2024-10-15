/*
 * Warpipes.c - Program to simulate a card game tournament between two children
 *
 * Author: Jacob Johnson
 * Date: 10/15/2024 
 *
 * Assignment: HW-Prog03
 * Course: CSCI 356
 * Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Constants for suits and ranks
#define NUM_CARDS 13  // 2-10 + Jack(11), Queen(12), King(13), Ace(14)
#define NUM_SUITS 4    // Spades(1), Hearts(2), Diamonds(3), Clubs(4)

// Function to generate a random card draw
void draw_card(int *rank, int *suit) {
    // Generate random rank and suit modulo + 1 to avoid 0
    *rank = rand() % NUM_CARDS + 1;
    *suit = rand() % NUM_SUITS + 1;
}

// Function to determine the winner based on card value and suit
int determine_winner(int rank1, int suit1, int rank2, int suit2) {
    if (rank1 > rank2) return 1;
    if (rank2 > rank1) return 2;
    
    // If ranks are equal, compare suits
    if (suit1 != suit2) {
        if (suit1 < suit2) return 1;  // Spades(1) > Hearts(2) > Diamonds(3) > Clubs(4)
        else return 2;
    }
    return 0;  // Tie
}

int main(int argc, char *argv[]) { 
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>\n", argv[0]);
        exit(1);
    }

    int rounds = atoi(argv[1]);
    if (rounds <= 0) {
        fprintf(stderr, "Number of rounds must be a positive integer.\n");
        exit(1);
    }

    int pipe1[2], pipe2[2];  // Pipes for communication with children
    pid_t pid1, pid2;

    // Seed the random number generator
    srand(time(NULL));

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork child 1
    if ((pid1 = fork()) == 0) {
        close(pipe1[0]);  // Close unused read end
        for (int i = 0; i < rounds; i++) {
            int rank, suit;
            draw_card(&rank, &suit);
            write(pipe1[1], &rank, sizeof(rank));
            write(pipe1[1], &suit, sizeof(suit));
        }
        close(pipe1[1]);  // Close write end
        exit(0);
    }

    // Fork child 2
    if ((pid2 = fork()) == 0) {
        close(pipe2[0]);  // Close unused read end
        for (int i = 0; i < rounds; i++) {
            int rank, suit;
            draw_card(&rank, &suit);
            write(pipe2[1], &rank, sizeof(rank));
            write(pipe2[1], &suit, sizeof(suit));
        }
        close(pipe2[1]);  // Close write end
        exit(0);
    }

    // Parent process
    close(pipe1[1]);  // Close unused write ends
    close(pipe2[1]);

    int score1 = 0, score2 = 0;

    printf("Child 1 PID: %d\n", pid1);
    printf("Child 2 PID: %d\n", pid2);
    printf("Beginning %d Rounds...\n", rounds);

    for (int i = 0; i < rounds; i++) {
        printf("---------------------------\n");
        printf("Round %d:\n", i + 1);

        int rank1, suit1, rank2, suit2;

        // Read from child 1
        read(pipe1[0], &rank1, sizeof(rank1));
        read(pipe1[0], &suit1, sizeof(suit1));
        printf("Child 1 draws %d of suit %d\n", rank1, suit1);

        // Read from child 2
        read(pipe2[0], &rank2, sizeof(rank2));
        read(pipe2[0], &suit2, sizeof(suit2));
        printf("Child 2 draws %d of suit %d\n", rank2, suit2);

        // Determine the winner
        int winner = determine_winner(rank1, suit1, rank2, suit2);
        if (winner == 1) {
            printf("Child 1 Wins!\n");
            score1++;
        } else if (winner == 2) {
            printf("Child 2 Wins!\n");
            score2++;
        } else {
            printf("It's a Tie!\n");
        }
    }

    // Close pipes
    close(pipe1[0]);
    close(pipe2[0]);

    // Wait for children to finish
    wait(NULL);
    wait(NULL);

    printf("---------------------------\n");
    printf("Results:\n");
    printf("Child 1: %d\n", score1);
    printf("Child 2: %d\n", score2);

    if (score1 > score2) {
        printf("Child 1 Wins the Tournament!\n");
    } else if (score2 > score1) {
        printf("Child 2 Wins the Tournament!\n");
    } else {
        printf("The Tournament is a Tie!\n");
    }

    return 0;
}
