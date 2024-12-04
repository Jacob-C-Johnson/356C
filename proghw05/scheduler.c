/*
 * scheduler.c - Program to simulate a process scheduler using First-Come First-Served (FCFS) and Preemptive Priority (PP) algorithms.
 *
 * Author: Jacob Johnson
 * Date: 12/04/2024 
 *
 * Assignment: HW-Prog05
 * Course: CSCI 356
 * Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

// Process structure
typedef struct Process {
    int pid;
    int arrival_time;
    int cpu_time;
    int priority;
    int remaining_time;
    int start_time;
    int completion_time;
    int waiting_time;
    int response_time;
    int last_run_time;  
    int last_enqueued_time;
    int last_aged_time;
} Process;

// Process statistics
typedef struct ProcessStats {
    double avg_waiting_time;
    double avg_response_time;
    double avg_turnaround_time;
    double cpu_usage;
} ProcessStats;

#define AGING_INTERVAL 8
#define MAX_PROCESSES 25

// Function prototypes
void run_fcfs(Process processes[], int count);
void run_pp(Process processes[], int count);
void print_stats(Process processes[], int count, int total_time);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: scheduler [FCFS | PP]\n");
        return 1;
    }

    Process processes[MAX_PROCESSES];
    int count = 0;
    int pid, arrival_time, cpu_time, priority;

    // Read the input
    while (1) {
        if (scanf("%d %d %d %d", &pid, &arrival_time, &cpu_time, &priority) != 4) break;
        if (pid == 0 && arrival_time == 0 && cpu_time == 0 && priority == 0) break;

        processes[count].pid = pid;
        processes[count].arrival_time = arrival_time;
        processes[count].cpu_time = cpu_time;
        processes[count].remaining_time = cpu_time;
        processes[count].priority = priority;  
        processes[count].start_time = -1;
        processes[count].completion_time = -1;
        processes[count].waiting_time = 0;
        processes[count].response_time = -1;
        processes[count].last_run_time = -1;
        processes[count].last_enqueued_time = -1;
        processes[count].last_aged_time = -1;

        count++;
    }

    // Run the simulation
    printf("Simulation starting:\n");
    if (strcmp(argv[1], "FCFS") == 0) {
        run_fcfs(processes, count);
    } else if (strcmp(argv[1], "PP") == 0) {
        run_pp(processes, count);
    } else {
        fprintf(stderr, "Invalid scheduling algorithm\n");
        return 1;
    }

    return 0;
}

void run_pp(Process processes[], int count) {
    queue ready_queue = newqueue();
    int current_time = 0;
    int completed = 0;
    int current_process_index = -1;
    
    // Initialize process timing
    for (int i = 0; i < count; i++) {
        processes[i].last_run_time = -1;
        processes[i].last_enqueued_time = -1;
        processes[i].last_aged_time = -1;
    }
    
    while (completed < count) {
        // Add new processes
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time == current_time) {
                printf("%d %d arriving\n", current_time, processes[i].pid);
                int* index = malloc(sizeof(int));
                *index = i;
                processes[i].last_enqueued_time = current_time;
                processes[i].last_run_time = -1;
                enqueue(ready_queue, index);
            }
        }

        // Modified aging section for run_pp():
        for (int i = 0; i < count; i++) {
            if (processes[i].remaining_time > 0 && 
                processes[i].arrival_time <= current_time &&
                i != current_process_index) {
                
                // Calculate waiting time
                int waiting_start;
                if (processes[i].last_run_time == -1) {
                    // Never run - count from arrival
                    waiting_start = processes[i].arrival_time;
                } else if (processes[i].last_aged_time != -1) {
                    // Count from last aging
                    waiting_start = processes[i].last_aged_time;
                } else {
                    // Count from last run
                    waiting_start = processes[i].last_run_time;
                }
                
                int waiting_time = current_time - waiting_start;
                
                // Age the process if it has been waiting for AGING_INTERVAL
                if (waiting_time >= AGING_INTERVAL && 
                    (processes[i].last_aged_time == -1 || 
                     current_time - processes[i].last_aged_time >= AGING_INTERVAL)) {
                    processes[i].priority++;
                    processes[i].last_aged_time = current_time;
                    printf("%d %d aging\n", current_time, processes[i].pid);
                }
            }
        }

        // Check if current process finished
        if (current_process_index != -1 && processes[current_process_index].remaining_time == 0) {
            printf("%d %d finished\n", current_time, processes[current_process_index].pid);
            processes[current_process_index].completion_time = current_time;
            completed++;
            current_process_index = -1;
            continue;
        }

        // Select highest priority process
        int highest_priority = current_process_index != -1 ? 
            processes[current_process_index].priority : -1;
        int selected_index = current_process_index;
        
        // Select the highest priority process
        queue temp_queue = newqueue();
        while (!isempty(ready_queue)) {
            int* index = dequeue(ready_queue);
            if (processes[*index].remaining_time > 0 && 
                processes[*index].priority > highest_priority) {
                if (selected_index != -1 && selected_index != current_process_index) {
                    int* prev = malloc(sizeof(int));
                    *prev = selected_index;
                    enqueue(temp_queue, prev);
                }
                highest_priority = processes[*index].priority;
                selected_index = *index;
            } else {
                enqueue(temp_queue, index);
            }
        }
        
        // Re-enqueue the remaining processes
        while (!isempty(temp_queue)) {
            enqueue(ready_queue, dequeue(temp_queue));
        }

        // Handle preemption
        if (selected_index != current_process_index) {
            if (current_process_index != -1) {
                int* index = malloc(sizeof(int));
                *index = current_process_index;
                processes[current_process_index].last_enqueued_time = current_time;
                enqueue(ready_queue, index);
            }
            current_process_index = selected_index;
        }

        // Run selected process
        if (current_process_index != -1) {
            if (processes[current_process_index].remaining_time == processes[current_process_index].cpu_time) {
                processes[current_process_index].start_time = current_time;
                processes[current_process_index].response_time = current_time - 
                    processes[current_process_index].arrival_time;
            }
            printf("%d %d running\n", current_time, processes[current_process_index].pid);
            processes[current_process_index].remaining_time--;
            processes[current_process_index].last_aged_time = -1;
            processes[current_process_index].last_run_time = current_time;
        }

        current_time++;
    }

    print_stats(processes, count, current_time);
}

void run_fcfs(Process processes[], int count) {
    queue ready_queue = newqueue();
    int current_time = 0;
    int completed = 0;
    int current_process_index = -1;

    // Run the simulation
    while (completed < count) {
        // Check for newly arrived processes
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time == current_time) {
                printf("%d %d arriving\n", current_time, processes[i].pid);
                int* index = malloc(sizeof(int));
                if (!index) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
                *index = i;
                enqueue(ready_queue, index);
            }
        }

        // Select the next process to run if no process is currently running
        if (current_process_index == -1 && !isempty(ready_queue)) {
            int* index = dequeue(ready_queue);
            if (index != NULL) {
                current_process_index = *index;
                free(index);

                // Start process execution
                processes[current_process_index].start_time = current_time;
                processes[current_process_index].response_time = current_time - processes[current_process_index].arrival_time;
                printf("%d %d running\n", current_time, processes[current_process_index].pid);
            }
        }

        // Execute the running process
        if (current_process_index != -1) {
            processes[current_process_index].remaining_time--;

            // If the process completes
            if (processes[current_process_index].remaining_time == 0) {
                printf("%d %d finished\n", current_time + 1, processes[current_process_index].pid);
                processes[current_process_index].completion_time = current_time + 1;
                completed++;

                // Calculate metrics
                processes[current_process_index].waiting_time =
                    processes[current_process_index].completion_time -
                    processes[current_process_index].arrival_time -
                    processes[current_process_index].cpu_time;

                current_process_index = -1; // Reset to select the next process
            }
        }

        current_time++;
    }

    // print the stats
    print_stats(processes, count, current_time);
}



void print_stats(Process processes[], int count, int total_time) {
    double total_waiting_time = 0;
    double total_response_time = 0;
    double total_turnaround_time = 0;
    int total_cpu_time = 0;
    
    for (int i = 0; i < count; i++) {
        int turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        total_waiting_time += turnaround_time - processes[i].cpu_time;
        total_response_time += processes[i].response_time;
        total_turnaround_time += turnaround_time;
        total_cpu_time += processes[i].cpu_time;
    }

    printf("Average waiting time: %.2f\n", total_waiting_time / count);
    printf("Average response time: %.2f\n", total_response_time / count);
    printf("Average turnaround time: %.2f\n", total_turnaround_time / count);
    printf("Average CPU usage: %.2f%%\n", 
           ((double)total_cpu_time / total_time) * 100);
}