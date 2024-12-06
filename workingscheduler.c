/*
Summary of File: This file implements a CPU scheduler that simulates different scheduling 
algorithms (FCFS and PP) given a process trace. The scheduler handles process execution 
scheduling and calculates various performance metrics.
Name: Devin Guo
Date: 12/02/2024
Course: CSCI356
Assignment: ProgHW05: CPU Scheduler Project
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

/* Process Control Block structure 
 * Holds all necessary information about a process including:
 * - Process identification and timing information
 * - Resource usage tracking
 * - State information for scheduling
 */
typedef struct {
    int pid;               // Process identifier
    int arrival_time;      // Time when process arrives in system
    int cpu_time;         // Total CPU time needed
    int remaining_time;   // CPU time still needed to complete
    int priority;         // Process priority (used in PP scheduling)
    int start_time;       // Time when process first gets CPU (-1 if not started)
    int wait_time;        // Time spent waiting for CPU
    int completion_time;  // Time when process completes (-1 if not completed)
    int preempted;        // Flag indicating if process was preempted
} PCB;

/* Statistics tracking structure
 * Maintains running totals of various performance metrics
 * Used to calculate final averages and CPU utilization
 */
typedef struct {
    int total_time;                  // Total simulation time
    int cpu_busy_time;               // Time CPU was busy
    int num_processes;               // Number of completed processes
    double total_waiting_time;       // Sum of all process waiting times
    double total_response_time;      // Sum of all process response times
    double total_turnaround_time;    // Sum of all process turnaround times
} SchedStats;

/* Function prototypes */
PCB* create_process(int pid, int arrival, int cpu_time, int priority);
void free_process(PCB* proc);
void print_event(int time, int pid, const char* event);
PCB* get_highest_priority_process(queue ready_queue);
void update_aging(queue ready_queue, int current_time);
void handle_process_completion(PCB* proc, SchedStats* stats, int current_time);
void print_statistics(SchedStats* stats);
void run_fcfs(queue job_queue);
void run_pp(queue job_queue);

/* Prints scheduling events to standard output
 * Formats output according to project specifications:
 * time    pid    event
 */
void print_event(int time, int pid, const char* event) {
    if (strcmp(event, "idle") == 0) {
        printf("%d\twaiting\n", time);
    } else {
        printf("%d\t%d\t%s\n", time, pid, event);
    }
}

/* Creates and initializes a new Process Control Block
 * Allocates memory and sets initial values for all PCB fields
 * Returns NULL on allocation failure
 */
PCB* create_process(int pid, int arrival, int cpu_time, int priority) {
    PCB* proc = (PCB*)malloc(sizeof(PCB));
    if (!proc) {
        fprintf(stderr, "Failed to allocate memory for process\n");
        exit(1);
    }
    
    // Initialize all PCB fields
    proc->pid = pid;
    proc->arrival_time = arrival;
    proc->cpu_time = cpu_time;
    proc->remaining_time = cpu_time;
    proc->priority = priority;
    proc->start_time = -1;          // -1 indicates not started
    proc->wait_time = 0;
    proc->completion_time = -1;      // -1 indicates not completed
    proc->preempted = 0;
    
    return proc;
}

/* Safely frees memory allocated for a PCB */
void free_process(PCB* proc) {
    if (proc) {
        free(proc);
    }
}

/* Finds and returns the highest priority process in the ready queue
 * Uses a temporary queue to maintain original queue order
 * Returns NULL if queue is empty
 */
PCB* get_highest_priority_process(queue ready_queue) {
    if (isempty(ready_queue)) {
        return NULL;
    }

    queue temp_queue = newqueue();
    PCB* highest_priority_proc = NULL;
    PCB* current_proc;

    // Search for highest priority process
    while (!isempty(ready_queue)) {
        current_proc = (PCB*)dequeue(ready_queue);
        if (!highest_priority_proc || current_proc->priority > highest_priority_proc->priority) {
            if (highest_priority_proc) {
                enqueue(temp_queue, highest_priority_proc);
            }
            highest_priority_proc = current_proc;
        } else {
            enqueue(temp_queue, current_proc);
        }
    }

    // Restore original queue minus highest priority process
    while (!isempty(temp_queue)) {
        enqueue(ready_queue, dequeue(temp_queue));
    }
    free(temp_queue);

    return highest_priority_proc;
}

/* Implements the aging mechanism for priority scheduling
 * Increases priority of processes that have waited too long
 * Prevents starvation by gradually increasing process priorities
 */
void update_aging(queue ready_queue, int current_time) {
    if (isempty(ready_queue)) {
        return;
    }

    queue temp_queue = newqueue();
    PCB* proc;

    while (!isempty(ready_queue)) {
        proc = (PCB*)dequeue(ready_queue);
        // Increase priority after waiting 8 time units
        if (proc->wait_time >= 8) {
            proc->priority++;
            print_event(current_time, proc->pid, "aging");
            proc->wait_time = 0;  // Reset wait counter after aging
        }
        enqueue(temp_queue, proc);
    }

    // Restore queue with updated priorities
    while (!isempty(temp_queue)) {
        enqueue(ready_queue, dequeue(temp_queue));
    }
    free(temp_queue);
}

/* Handles process completion and updates statistics
 * Calculates waiting time, response time, and turnaround time
 * Updates global statistics tracking structure
 */
void handle_process_completion(PCB* proc, SchedStats* stats, int current_time) {
    proc->completion_time = current_time;
    
    // Calculate performance metrics
    int wait_time = current_time - proc->arrival_time - proc->cpu_time;
    int response_time = proc->start_time - proc->arrival_time;
    int turnaround_time = current_time - proc->arrival_time;
    
    // Update global statistics
    stats->total_waiting_time += wait_time;
    stats->total_response_time += response_time;
    stats->total_turnaround_time += turnaround_time;
    stats->num_processes++;
}

/* Prints final scheduling statistics
 * Calculates and displays average times and CPU utilization
 */
void print_statistics(SchedStats* stats) {
    if (stats->num_processes == 0) {
        printf("No processes completed\n");
        return;
    }

    // Calculate and print averages
    printf("Scheduling Statistics:\n");
    printf("Average waiting time: %.2f\n", 
           stats->total_waiting_time / stats->num_processes);
    printf("Average response time: %.2f\n", 
           stats->total_response_time / stats->num_processes);
    printf("Average turnaround time: %.2f\n", 
           stats->total_turnaround_time / stats->num_processes);
    printf("CPU usage: %.2f%%\n", 
           (stats->cpu_busy_time * 100.0) / stats->total_time);
}

/* First Come First Serve (FCFS) Scheduling Algorithm Implementation
 * - Non-preemptive scheduling
 * - Processes run to completion in arrival order
 * - Maintains ready queue of arrived but not running processes
 */
void run_fcfs(queue job_queue) {
    queue ready_queue = newqueue();
    SchedStats stats = {0, 0, 0, 0.0, 0.0, 0.0};
    PCB* current_proc = NULL;
    int current_time = 0;

    // Main scheduling loop
    while (!isempty(job_queue) || !isempty(ready_queue) || current_proc) {
        // Move newly arrived processes to ready queue
        while (!isempty(job_queue)) {
            PCB* proc = (PCB*)peek(job_queue);
            if (proc->arrival_time > current_time) {
                break;
            }
            proc = (PCB*)dequeue(job_queue);
            enqueue(ready_queue, proc);
            print_event(current_time, proc->pid, "arriving");
        }

        // Start new process if CPU is idle
        if (!current_proc && !isempty(ready_queue)) {
            current_proc = (PCB*)dequeue(ready_queue);
            if (current_proc->start_time == -1) {
                current_proc->start_time = current_time;
            }
        }

        // Execute current process or idle
        if (current_proc) {
            print_event(current_time, current_proc->pid, "running");
            current_proc->remaining_time--;
            stats.cpu_busy_time++;

            // Handle process completion
            if (current_proc->remaining_time == 0) {
                print_event(current_time + 1, current_proc->pid, "finished");
                handle_process_completion(current_proc, &stats, current_time + 1);
                free_process(current_proc);
                current_proc = NULL;
            }
        } else {
            print_event(current_time, 0, "idle");
        }

        // Update waiting time for processes in ready queue
        queue temp_queue = newqueue();
        while (!isempty(ready_queue)) {
            PCB* proc = (PCB*)dequeue(ready_queue);
            proc->wait_time++;
            enqueue(temp_queue, proc);
        }
        while (!isempty(temp_queue)) {
            enqueue(ready_queue, dequeue(temp_queue));
        }
        free(temp_queue);

        current_time++;
        stats.total_time = current_time;
    }

    print_statistics(&stats);
    free(ready_queue);
}

/* Preemptive Priority (PP) Scheduling Algorithm Implementation
 * - Preemptive scheduling based on priority
 * - Includes aging mechanism to prevent starvation
 * - Higher priority processes preempt lower priority ones
 */
void run_pp(queue job_queue) {
    queue ready_queue = newqueue();
    SchedStats stats = {0, 0, 0, 0.0, 0.0, 0.0};
    PCB* current_proc = NULL;
    int current_time = 0;

    // Main scheduling loop
    while (!isempty(job_queue) || !isempty(ready_queue) || current_proc) {
        // Handle new process arrivals
        while (!isempty(job_queue)) {
            PCB* proc = (PCB*)peek(job_queue);
            if (proc->arrival_time > current_time) {
                break;
            }
            proc = (PCB*)dequeue(job_queue);
            enqueue(ready_queue, proc);
            print_event(current_time, proc->pid, "arriving");
        }

        // Update process priorities through aging
        update_aging(ready_queue, current_time);

        // Check for preemption by higher priority process
        PCB* highest_priority_proc = get_highest_priority_process(ready_queue);
        if (highest_priority_proc && (!current_proc || 
            highest_priority_proc->priority > current_proc->priority)) {
            if (current_proc) {
                current_proc->preempted = 1;
                current_proc->wait_time = 0;  // Reset wait time when preempted
                enqueue(ready_queue, current_proc);
            }
            current_proc = highest_priority_proc;
            current_proc->wait_time = 0;  // Reset wait time when getting CPU
            if (current_proc->start_time == -1) {
                current_proc->start_time = current_time;
            }
        } else if (highest_priority_proc) {
            enqueue(ready_queue, highest_priority_proc);
        }

        // Execute current process or idle
        if (current_proc) {
            print_event(current_time, current_proc->pid, "running");
            current_proc->remaining_time--;
            stats.cpu_busy_time++;

            // Handle process completion
            if (current_proc->remaining_time == 0) {
                print_event(current_time + 1, current_proc->pid, "finished");
                handle_process_completion(current_proc, &stats, current_time + 1);
                free_process(current_proc);
                current_proc = NULL;
            }
        } else {
            print_event(current_time, 0, "idle");
        }

        // Update waiting time for ready queue processes
        queue temp_queue = newqueue();
        while (!isempty(ready_queue)) {
            PCB* proc = (PCB*)dequeue(ready_queue);
            proc->wait_time++;
            enqueue(temp_queue, proc);
        }
        while (!isempty(temp_queue)) {
            enqueue(ready_queue, dequeue(temp_queue));
        }
        free(temp_queue);

        current_time++;
        stats.total_time = current_time;
    }

    print_statistics(&stats);
    free(ready_queue);
}

/* Main program entry point
 * Handles command line arguments and input processing
 * Initiates appropriate scheduling algorithm based on input
 */
int main(int argc, char* argv[]) {
    // Verify correct command line usage
    if (argc != 2) {
        printf("Usage: %s [FCFS | PP]\n", argv[0]);
        return 1;
    }

    // Print input format instructions
    printf("Enter tasks in the format: ");
    if (strcmp(argv[1], "FCFS") == 0) {
        printf("<id> <arrival_time> <burst_time>\n");
    } else if (strcmp(argv[1], "PP") == 0) {
        printf("<id> <arrival_time> <burst_time> <priority>\n");
    } else {
        printf("Invalid algorithm: %s\n", argv[1]);
        return 1;
    }
    printf("Enter 0 for everything when you're done.\n");

    // Read and store process information
    queue job_queue = newqueue();
    int pid, arrival_time, cpu_time, priority;

    while (1) {
        if (scanf("%d", &pid) != 1 || pid == 0) {
            break;
        }
        
        if (scanf("%d %d", &arrival_time, &cpu_time) != 2) {
            fprintf(stderr, "Error reading process information\n");
            return 1;
        }

        priority = 0;  // Default priority for FCFS
        if (strcmp(argv[1], "PP") == 0) {
            if (scanf("%d", &priority) != 1) {
                fprintf(stderr, "Error reading priority\n");
                return 1;
            }
        }

        PCB* proc = create_process(pid, arrival_time, cpu_time, priority);
        enqueue(job_queue, proc);
    }

    // Run appropriate scheduling algorithm
    if (strcmp(argv[1], "FCFS") == 0) {
        run_fcfs(job_queue);
    } else if (strcmp(argv[1], "PP") == 0) {
        run_pp(job_queue);
    }

    // Clean up allocated memory
    free(job_queue);
    return 0;
}