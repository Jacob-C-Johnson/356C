#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCKET_PATH "/tmp/war_socket"

typedef struct {
    int thread_socket;
} thread_data_t;

void *client_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    // Connect to the server as a client
    sleep(1); // Give the parent a moment to set up the listener

    int client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sockfd < 0) {
        perror("client socket failed...");
        pthread_exit(NULL);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (connect(client_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed...");
        close(client_sockfd);
        pthread_exit(NULL);
    }

    // Send a message to the server
    const char *message = "Hello from Client!";
    write(client_sockfd, message, strlen(message) + 1);

    close(client_sockfd);
    pthread_exit(NULL);
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

    unlink(SOCKET_PATH);

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed...");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

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
    pthread_create(&thread1, NULL, client_thread, (void *)&data1);
    pthread_create(&thread2, NULL, client_thread, (void *)&data2);

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

    // Read messages from the clients
    char buffer[100];
    ssize_t bytes_read = read(data1.thread_socket, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Parent received from Client 1: %s\n", buffer);
    }

    bytes_read = read(data2.thread_socket, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Parent received from Client 2: %s\n", buffer);
    }

    // Close the accepted connections
    close(data1.thread_socket);
    close(data2.thread_socket);

    // Clean up the server socket
    close(sockfd);
    unlink(SOCKET_PATH);

    // Wait for the client threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}