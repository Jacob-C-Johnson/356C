#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

typedef struct {
    int pid;
    int arrival_time;
    int cpu_time;
    int priority;
    int start_time;
    int completion_time;
    int remaining_time;
    int response_time;
    int waiting_time;
    int turnaround_time;
} Process;

// Function prototypes
void fcfs_scheduler(queue process_queue, Process* process_list[], int num_processes);
void pp_scheduler(queue process_queue, Process* process_list[], int num_processes);
void print_summary(Process* process_list[], int num_processes, int total_time, int cpu_time);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [FCFS | PP]\n", argv[0]);
        return 1;
    }

    char* algorithm = argv[1];
    queue process_queue = newqueue();
    Process* process_list[25];
    int num_processes = 0;

    // Read input
    while (1) {
        int pid, arrival_time, cpu_time, priority;
        scanf("%d %d %d %d", &pid, &arrival_time, &cpu_time, &priority);
        if (pid == 0 && arrival_time == 0 && cpu_time == 0 && priority == 0) break;

        Process* process = (Process*)malloc(sizeof(Process));
        process->pid = pid;
        process->arrival_time = arrival_time;
        process->cpu_time = cpu_time;
        process->priority = priority;
        process->start_time = -1;
        process->completion_time = -1;
        process->remaining_time = cpu_time;
        process->response_time = -1;
        process->waiting_time = 0;
        process->turnaround_time = 0;

        enqueue(process_queue, process);
        process_list[num_processes++] = process;
    }

    // Choose scheduling algorithm
    if (strcmp(algorithm, "FCFS") == 0) {
        fcfs_scheduler(process_queue, process_list, num_processes);
    } else if (strcmp(algorithm, "PP") == 0) {
        pp_scheduler(process_queue, process_list, num_processes);
    } else {
        fprintf(stderr, "Invalid algorithm: %s\n", algorithm);
        return 1;
    }

    return 0;
}

// First Come First Serve Scheduler
void fcfs_scheduler(queue process_queue, Process* process_list[], int num_processes) {
    int current_time = 0, cpu_time = 0;
    Process* process;

    printf("Simulation starting:\n");
    while (!isempty(process_queue)) {
        process = (Process*)dequeue(process_queue);

        // Process arrival
        if (current_time < process->arrival_time) {
            printf("%d\tCPU idle\n", current_time);
            current_time = process->arrival_time;
        }
        printf("%d\t%d\tarriving\n", current_time, process->pid);

        // Start processing
        process->start_time = current_time;
        printf("%d\t%d\trunning\n", current_time, process->pid);

        current_time += process->cpu_time;
        process->completion_time = current_time;

        // Process completion
        printf("%d\t%d\tfinished\n", current_time, process->pid);

        process->turnaround_time = process->completion_time - process->arrival_time;
        process->waiting_time = process->turnaround_time - process->cpu_time;
        process->response_time = process->start_time - process->arrival_time;

        cpu_time += process->cpu_time;
    }

    print_summary(process_list, num_processes, current_time, cpu_time);
}

// Preemptive Priority Scheduler with Aging
void pp_scheduler(queue process_queue, Process* process_list[], int num_processes) {
    int current_time = 0, cpu_time = 0;
    Process* running_process = NULL;
    queue ready_queue = newqueue();

    printf("Simulation starting:\n");
    while (!isempty(process_queue) || running_process || !isempty(ready_queue)) {
        // Check for arrivals
        while (!isempty(process_queue)) {
            Process* process = (Process*)peek(process_queue);
            if (process->arrival_time > current_time) break;
            dequeue(process_queue);
            enqueue(ready_queue, process);
            printf("%d\t%d\tarriving\n", current_time, process->pid);
        }

        // Apply aging every 8 ms
        if (current_time % 8 == 0) {
            queue temp_queue = newqueue();
            while (!isempty(ready_queue)) {
                Process* process = (Process*)dequeue(ready_queue);
                process->priority++;
                enqueue(temp_queue, process);
                printf("%d\t%d\taging\n", current_time, process->pid);
            }
            while (!isempty(temp_queue)) enqueue(ready_queue, dequeue(temp_queue));
        }

        // Select the highest-priority process
        if (!running_process || (!isempty(ready_queue) && ((Process*)peek(ready_queue))->priority > running_process->priority)) {
            if (running_process && running_process->remaining_time > 0) {
                enqueue(ready_queue, running_process);
            }
            running_process = (Process*)dequeue(ready_queue);
            if (running_process->response_time == -1) {
                running_process->response_time = current_time - running_process->arrival_time;
            }
            printf("%d\t%d\trunning\n", current_time, running_process->pid);
        }

        // Process execution
        if (running_process) {
            running_process->remaining_time--;
            cpu_time++;
            if (running_process->remaining_time == 0) {
                printf("%d\t%d\tfinished\n", current_time + 1, running_process->pid);
                running_process->completion_time = current_time + 1;
                running_process->turnaround_time = running_process->completion_time - running_process->arrival_time;
                running_process->waiting_time = running_process->turnaround_time - running_process->cpu_time;
                running_process = NULL;
            }
        } else {
            printf("%d\tCPU idle\n", current_time);
        }

        current_time++;
    }

    print_summary(process_list, num_processes, current_time, cpu_time);
}

// Summary Metrics
void print_summary(Process* process_list[], int num_processes, int total_time, int cpu_time) {
    double avg_waiting_time = 0, avg_response_time = 0, avg_turnaround_time = 0;

    for (int i = 0; i < num_processes; i++) {
        avg_waiting_time += process_list[i]->waiting_time;
        avg_response_time += process_list[i]->response_time;
        avg_turnaround_time += process_list[i]->turnaround_time;
    }

    avg_waiting_time /= num_processes;
    avg_response_time /= num_processes;
    avg_turnaround_time /= num_processes;

    printf("Average waiting time: %.2f\n", avg_waiting_time);
    printf("Average response time: %.2f\n", avg_response_time);
    printf("Average turnaround time: %.2f\n", avg_turnaround_time);
    printf("Average CPU usage: %.2f%%\n", (double)cpu_time / total_time * 100);
}
