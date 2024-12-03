#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

typedef struct {
    int pid;
    int arrival_time;
    int cpu_time;
    int priority; // Only valid for PP
    int start_time;
    int completion_time;
    int remaining_time;
} Process;

// Function prototypes
void fcfs_scheduler(queue process_queue);
void pp_scheduler(queue process_queue);
void simulate_clock(queue process_queue, int is_pp);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [FCFS | PP]\n", argv[0]);
        return 1;
    }

    // Read scheduling algorithm from command line
    char* algorithm = argv[1];

    // Parse input into a queue of processes
    queue process_queue = newqueue();
    while (1) {
        int pid, arrival_time, cpu_time, priority;
        scanf("%d %d %d %d", &pid, &arrival_time, &cpu_time, &priority);

        if (pid == 0 && arrival_time == 0 && cpu_time == 0 && priority == 0) break;

        Process* process = (Process*)malloc(sizeof(Process));
        process->pid = pid;
        process->arrival_time = arrival_time;
        process->cpu_time = cpu_time;
        process->priority = priority;
        process->remaining_time = cpu_time;
        process->start_time = -1;
        process->completion_time = -1;

        enqueue(process_queue, process);
    }

    // Run the appropriate scheduler
    if (strcmp(algorithm, "FCFS") == 0) {
        fcfs_scheduler(process_queue);
    } else if (strcmp(algorithm, "PP") == 0) {
        pp_scheduler(process_queue);
    } else {
        fprintf(stderr, "Invalid algorithm: %s\n", algorithm);
        return 1;
    }

    return 0;
}

// FCFS Scheduler Implementation
void fcfs_scheduler(queue process_queue) {
    simulate_clock(process_queue, 0);
}

// Preemptive Priority Scheduler Implementation with Aging
void pp_scheduler(queue process_queue) {
    simulate_clock(process_queue, 1);
}

// Simulates clock-driven scheduling
void simulate_clock(queue process_queue, int is_pp) {
    int current_time = 0;
    Process* running_process = NULL;
    queue ready_queue = newqueue();

    while (!isempty(process_queue) || running_process != NULL || !isempty(ready_queue)) {
        // Check for process arrivals
        while (!isempty(process_queue)) {
            Process* process = (Process*)peek(process_queue);
            if (process->arrival_time > current_time) break;
            dequeue(process_queue);
            enqueue(ready_queue, process);
            printf("%d\t%d\tarriving\n", current_time, process->pid);
        }

        // Preemptive priority: Re-sort ready queue based on priority and aging
        if (is_pp && !isempty(ready_queue)) {
            queue temp_queue = newqueue();
            while (!isempty(ready_queue)) {
                Process* process = (Process*)dequeue(ready_queue);
                process->priority += (current_time % 8 == 0 && current_time != 0) ? 1 : 0;
                enqueue(temp_queue, process);
            }

            // Sort the queue by priority (descending) and arrival time
            while (!isempty(temp_queue)) {
                Process* process = (Process*)dequeue(temp_queue);
                if (isempty(ready_queue)) {
                    enqueue(ready_queue, process);
                } else {
                    queue sorted_queue = newqueue();
                    while (!isempty(ready_queue)) {
                        Process* front = (Process*)dequeue(ready_queue);
                        if (process->priority > front->priority) {
                            enqueue(sorted_queue, process);
                            enqueue(sorted_queue, front);
                            break;
                        }
                        enqueue(sorted_queue, front);
                    }
                    while (!isempty(ready_queue)) enqueue(sorted_queue, dequeue(ready_queue));
                    enqueue(ready_queue, dequeue(sorted_queue));
                }
            }
        }

        // Process execution
        if (running_process == NULL && !isempty(ready_queue)) {
            running_process = (Process*)dequeue(ready_queue);
            if (running_process->start_time == -1) {
                running_process->start_time = current_time;
            }
            printf("%d\t%d\trunning\n", current_time, running_process->pid);
        }

        // Check if the running process has completed
        if (running_process != NULL) {
            running_process->remaining_time--;
            if (running_process->remaining_time == 0) {
                running_process->completion_time = current_time + 1;
                printf("%d\t%d\tfinished\n", current_time + 1, running_process->pid);
                free(running_process);
                running_process = NULL;
            }
        } else {
            printf("%d\tCPU idle\n", current_time);
        }

        current_time++;
    }
}
