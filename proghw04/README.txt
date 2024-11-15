/*
 * war_networked.c - Program to simulate a card game tournament between two children using sockets and pthreads
 *
 * Author: Jacob Johnson
 * Date: 11/14/2024 
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
#include <sys/wait.h>

// Struct to pass in the socket information
typedef struct {
    int thread_socket;
} thread_data_t;

// Card structure
typedef struct {
    int rank;   // 2-14 (2-10, Jack=11, Queen=12, King=13, Ace=14)
    char suit;  // 'S', 'H', 'D', 'C' (Spades, Hearts, Diamonds, Clubs)
} Card;

// Function to draw a random card
Card draw_card() {
    Card card;
    card.rank = (rand() % 13) + 2; // Generate rank between 2 and 14
    int suit_index = rand() % 4;   // Randomize suit index

    // Map suit_index to suit character
    switch (suit_index) {
        case 0: card.suit = 'Spades'; break; // Spades
        case 1: card.suit = 'Hearts'; break; // Hearts
        case 2: card.suit = 'Diamonds'; break; // Diamonds
        case 3: card.suit = 'Clubs'; break; // Clubs
    }

    return card;
}

void print_card(const Card *card) {
    const char *rank_names[] = {"", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
    printf("%s of %c", rank_names[card->rank], card->suit);
}

// Function to resolve ties
int resolve_tie(Card card1, Card card2) {
    if (card1.rank > card2.rank) {
        return 1;
    } else if (card1.rank < card2.rank) {
        return 2;
    } else {
        return 0;
    }
}


// Thread function for child threads
void *child_thread(void *arg) {
    int socket_fd = *(int *)arg;
    while (1) {
        char buffer[256];
        read(socket_fd, buffer, sizeof(buffer));
        if (strcmp(buffer, "QUIT") == 0) {
            break; // Exit the thread
        } else if (strcmp(buffer, "DRAW") == 0) {
            Card card = draw_card();
            write(socket_fd, &card, sizeof(Card)); // Send card to parent
        }
    }
    return NULL;
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

    unlink("/tmp/war_socket");

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed...");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/tmp/war_socket");

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 2) < 0) {
        perror("listen failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t thread1, thread2;
    thread_data_t data1, data2;

    // Create client threads
    pthread_create(&thread1, NULL, child_thread, (void *)&data1);
    pthread_create(&thread2, NULL, child_thread, (void *)&data2);

    // Accept connections from the clients
    data1.thread_socket = accept(sockfd, NULL, NULL);
    if (data1.thread_socket < 0) {
        perror("accept failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    data2.thread_socket = accept(sockfd, NULL, NULL);
    if (data2.thread_socket < 0) {
        perror("accept failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Play the game for the specified number of rounds
    for (int i = 0; i < rounds; i++) {
        write(data1.thread_socket, "DRAW", 5);
        write(data2.thread_socket, "DRAW", 5);

        Card card1, card2;
        read(data1.thread_socket, &card1, sizeof(Card));
        read(data2.thread_socket, &card2, sizeof(Card));

        int winner = resolve_tie(card1, card2);
        if (winner == 1) {
            write(data1.thread_socket, "WIN", 4);
            write(data2.thread_socket, "LOSE", 5);
        } else if (winner == 2) {
            write(data1.thread_socket, "LOSE", 5);
            write(data2.thread_socket, "WIN", 4);
        } else {
            write(data1.thread_socket, "TIE", 4);
            write(data2.thread_socket, "TIE", 4);
        }
    }

    // Send quit message to the clients
    write(data1.thread_socket, "QUIT", 5);
    write(data2.thread_socket, "QUIT", 5);

    // Close the accepted connections
    close(data1.thread_socket);
    close(data2.thread_socket);

    // Clean up the server socket
    close(sockfd);
    unlink("/tmp/war_socket");

    // Wait for the client threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}