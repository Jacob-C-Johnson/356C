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

// Socket file path
#define SOCKET_PATH "/tmp/war_socket"

// Function prototypes
int draw_card();
void resolve_tie(int c1_card, int c2_card, int *winner);

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>\n", argv[0]);
        exit(1);
    }

    // Ensure rounds argument is a positive integer
    int rounds = atoi(argv[1]);
    if (rounds <= 0) {
        fprintf(stderr, "Number of rounds must be a positive integer.\n");
        exit(1);
    }

    // Unlink the socket file if it already exists
    unlink(SOCKET_PATH);

    // Initialize the server socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed...");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind the server socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 1) < 0) {
        perror("listen failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Fork a child process to act as a client
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed...");
        close(sockfd);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: connect to the server as a client
        sleep(1); // Give the parent a moment to set up the listener

        int client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_sockfd < 0) {
            perror("client socket failed...");
            exit(EXIT_FAILURE);
        }

        if (connect(client_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("connect failed...");
            close(client_sockfd);
            exit(EXIT_FAILURE);
        }

        // Send a message to the server
        const char *message = "Hello from Child!";
        write(client_sockfd, message, strlen(message) + 1);

        close(client_sockfd);
        exit(0);
    } else {
        // Parent process: accept the connection from the child
        int c1 = accept(sockfd, NULL, NULL);
        if (c1 < 0) {
            perror("accept failed...");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Read message from the child
        char buffer[100];
        ssize_t bytes_read = read(c1, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Parent received: %s\n", buffer);
        }

        // Close the accepted connection
        close(c1);
    }

    // Clean up the server socket
    close(sockfd);
    unlink(SOCKET_PATH);

    return 0;
}

int draw_card() {
    srand(time(NULL));
    return rand() % 13 + 1;
}