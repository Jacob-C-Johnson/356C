/*
 * war_networked.c - Program to simulate a card game tournament between two children using Pthreads and Sockets for child-parent communication.
 *
 * Author: Jacob Johnson
 * Date: 11/15/2024 
 *
 * Assignment: HW-Prog04
 * Course: CSCI 356
 * Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Struct to pass in the socket information
typedef struct {
    int thread_socket;
} thread_data_t;

// Card structure
typedef struct {
    int rank;   // 2-14 (2-10, Jack=11, Queen=12, King=13, Ace=14)
    char suit[10];  
} Card;

// Function to draw a random card
Card draw_card() {
    Card card;
    card.rank = (rand() % 13) + 2; // Generate rank between 2 and 14
    int suit_index = rand() % 4;   // Randomize suit index

    // Map suit_index to suit string
    switch (suit_index) {
        case 0: strcpy(card.suit, "Spades"); break;
        case 1: strcpy(card.suit, "Hearts"); break;
        case 2: strcpy(card.suit, "Diamonds"); break;
        case 3: strcpy(card.suit, "Clubs"); break;
    }

    return card;
}

// Suit precedence for tie resolution
int suit_precedence(char *suit) {
    if (strcmp(suit, "Spades") == 0) return 4;
    if (strcmp(suit, "Hearts") == 0) return 3;
    if (strcmp(suit, "Diamonds") == 0) return 2;
    if (strcmp(suit, "Clubs") == 0) return 1;
    return 0;
}

// Format the cards
char *format_card(Card card) {
    char *formatted = malloc(20);
    char *rank;
    switch (card.rank) {
        case 11: rank = "Jack"; break;
        case 12: rank = "Queen"; break;
        case 13: rank = "King"; break;
        case 14: rank = "Ace"; break;
        default: rank = malloc(3);
                 sprintf(rank, "%d", card.rank);
    }
    sprintf(formatted, "%s", rank);
    return formatted;
}

// Thread function for child threads
void *child_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg; // Get the socket from the thread data
    int socket_fd = data->thread_socket; // Child side
    while (1) {
        char buffer[256]; // Buffer for reading commands
        read(socket_fd, buffer, sizeof(buffer)); // Read command from parent
        if (strcmp(buffer, "QUIT") == 0) {
            break; // Exit the thread
        } else if (strcmp(buffer, "DRAW") == 0) {
            Card card = draw_card();
            write(socket_fd, &card, sizeof(Card)); // Send card to parent
        }
    }
    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse the number of rounds
    int rounds = atoi(argv[1]);
    if (rounds <= 0) {
        fprintf(stderr, "Number of rounds must be greater than 0.");
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand(time(NULL));

    // Create sockets for parent-child communication
    int sockets[2][2]; // One socket pair per child
    for (int i = 0; i < 2; i++) {
        // Create a socket pair
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0) {
            perror("socketpair");
            exit(EXIT_FAILURE); // Exit if socket creation fails
        }
    }

    // Create child threads
    pthread_t threads[2];
    thread_data_t thread_data[2];
    for (int i = 0; i < 2; i++) {
        thread_data[i].thread_socket = sockets[i][1]; // Child side
        // Create the child thread
        if (pthread_create(&threads[i], NULL, child_thread, &thread_data[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Print the thread IDs instead of process IDs for child threads
    printf("\nChild 1 PID: %lu\n", threads[0]);
    printf("\nChild 2 PID: %lu\n", threads[1]);

    // Begin the tournament
    printf("\nBeginning %d Rounds…\n", rounds);
    printf("\nFight!\n");
    printf("---------------------------\n");

    int wins[2] = {0, 0}; // Track wins for each child

    for (int round = 1; round <= rounds; round++) {
        printf("\nRound %d:\n", round);

        // Signal both children to draw cards
        for (int i = 0; i < 2; i++) {
            write(sockets[i][0], "DRAW", strlen("DRAW") + 1);
        }

        // Receive cards from both children
        Card cards[2];
        for (int i = 0; i < 2; i++) {
            read(sockets[i][0], &cards[i], sizeof(Card));
        }

        // Print the draws (ranks only)
        printf("\nChild 1 draws %s \n", format_card(cards[0]));
        printf("\nChild 2 draws %s \n", format_card(cards[1]));

        // Determine the winner
        if (cards[0].rank > cards[1].rank) {
            printf("\nChild 1 Wins!\n");
            wins[0]++;
        } else if (cards[1].rank > cards[0].rank) {
            printf("\nChild 2 Wins!\n");
            wins[1]++;
        } else { // Handle tie (show suits)
            printf("\nChecking suits…\n");
            printf("\nChild 1 draws suit %s %s \n", format_card(cards[0]), cards[0].suit);
            printf("\nChild 2 draws suit %s %s \n", format_card(cards[1]), cards[1].suit);
            if (suit_precedence(cards[0].suit) > suit_precedence(cards[1].suit)) {
                printf("\nChild 1 Wins!\n");
                wins[0]++;
            } else {
                printf("\nChild 2 Wins!\n");
                wins[1]++;
            }
        }
        printf("\n---------------------------\n");
    }

    // Check for sudden death if tied
    if (wins[0] == wins[1]) {
        printf("\nSudden Death Round!\n");

        // Signal both children to draw cards
        for (int i = 0; i < 2; i++) {
            write(sockets[i][0], "DRAW", strlen("DRAW") + 1);
        }

        // Receive cards from both children
        Card cards[2];
        for (int i = 0; i < 2; i++) {
            read(sockets[i][0], &cards[i], sizeof(Card));
        }

        // Print the sudden death results (ranks only unless tied)
        printf("\nChild 1 draws %s \n", format_card(cards[0]));
        printf("\nChild 2 draws %s \n", format_card(cards[1]));
        if (cards[0].rank > cards[1].rank || 
            (cards[0].rank == cards[1].rank && suit_precedence(cards[0].suit) > suit_precedence(cards[1].suit))) {
            wins[0]++;
            printf("\nChild 1 Wins!\n");
        } else {
            wins[1]++;
            printf("\nChild 2 Wins!\n");
        }
    } 
    else {
        printf("\n---------------------------\n");
        printf("\nResults:\n \nChild 1: %d \n \nChild 2: %d \n", wins[0], wins[1]);
        if (wins[0] > wins[1]) {
            printf("\nChild 1 Wins!\n");
        } else {
            printf("\nChild 2 Wins!\n");
        }
    }

    // Signal children to quit
    for (int i = 0; i < 2; i++) {
        write(sockets[i][0], "QUIT", strlen("QUIT") + 1);
    }

    // Wait for threads to exit
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}