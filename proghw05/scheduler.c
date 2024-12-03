#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

typedef struct {
    int pid;
    int arrival_time;
    int cpu_time;
    int priority; // Only used for PP
    int start_time;
    int completion_time;
    int remaining_time;
    int waiting_time;
    int response_time;
    int turnaround_time;
} Process;

// Function prototypes
void fcfs_scheduler(queue process_queue, Process* process_list[], int num_processes);
void pp_scheduler(queue process_queue, Process* process_list[], int num_processes);
void print_summary(Process* processes[], int num_processes, int total_time, int cpu_time);

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
        process->remaining_time = cpu_time;
        process->start_time = -1;
        process->completion_time = -1;
        process->waiting_time = 0;
        process->response_time = -1;
        process->turnaround_time = 0;

        enqueue(process_queue, process);
        process_list[num_processes++] = process;
    }

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

void fcfs_scheduler(queue process_queue, Process* process_list[], int num_processes) {
    int current_time = 0, cpu_time = 0;
    Process* process;
    queue ready_queue = newqueue();

    while (!isempty(process_queue) || !isempty(ready_queue)) {
        while (!isempty(process_queue)) {
            process = (Process*)peek(process_queue);
            if (process->arrival_time > current_time) break;
            dequeue(process_queue);
            enqueue(ready_queue, process);
            printf("%d\t%d\tarriving\n", current_time, process->pid);
        }

        if (!isempty(ready_queue)) {
            process = (Process*)dequeue(ready_queue);
            if (process->response_time == -1) process->response_time = current_time - process->arrival_time;

            printf("%d\t%d\trunning\n", current_time, process->pid);
            cpu_time += process->remaining_time;
            current_time += process->remaining_time;
            process->remaining_time = 0;

            process->completion_time = current_time;
            process->turnaround_time = process->completion_time - process->arrival_time;
            process->waiting_time = process->turnaround_time - process->cpu_time;

            printf("%d\t%d\tfinished\n", current_time, process->pid);
        } else {
            printf("%d\tCPU idle\n", current_time);
            current_time++;
        }
    }

    print_summary(process_list, num_processes, current_time, cpu_time);
}

void pp_scheduler(queue process_queue, Process* process_list[], int num_processes) {
    int current_time = 0, cpu_time = 0;
    Process* running_process = NULL;
    queue ready_queue = newqueue();

    while (!isempty(process_queue) || running_process || !isempty(ready_queue)) {
        while (!isempty(process_queue)) {
            Process* process = (Process*)peek(process_queue);
            if (process->arrival_time > current_time) break;
            dequeue(process_queue);
            enqueue(ready_queue, process);
            printf("%d\t%d\tarriving\n", current_time, process->pid);
        }

        if (running_process && running_process->remaining_time == 0) {
            printf("%d\t%d\tfinished\n", current_time, running_process->pid);
            running_process->completion_time = current_time;
            running_process->turnaround_time = running_process->completion_time - running_process->arrival_time;
            running_process->waiting_time = running_process->turnaround_time - running_process->cpu_time;
            running_process = NULL;
        }

        if (!running_process && !isempty(ready_queue)) {
            running_process = (Process*)dequeue(ready_queue);
            if (running_process->response_time == -1) running_process->response_time = current_time - running_process->arrival_time;
            printf("%d\t%d\trunning\n", current_time, running_process->pid);
        }

        current_time++;
        if (running_process) running_process->remaining_time--;
    }

    print_summary(process_list, num_processes, current_time, cpu_time);
}

void print_summary(Process* processes[], int num_processes, int total_time, int cpu_time) {
    double avg_waiting_time = 0, avg_response_time = 0, avg_turnaround_time = 0;
    for (int i = 0; i < num_processes; i++) {
        avg_waiting_time += processes[i]->waiting_time;
        avg_response_time += processes[i]->response_time;
        avg_turnaround_time += processes[i]->turnaround_time;
    }
    avg_waiting_time /= num_processes;
    avg_response_time /= num_processes;
    avg_turnaround_time /= num_processes;

    printf("Average waiting time: %.2f\n", avg_waiting_time);
    printf("Average response time: %.2f\n", avg_response_time);
    printf("Average turnaround time: %.2f\n", avg_turnaround_time);
    printf("Average CPU usage: %.2f%%\n", (double)cpu_time / total_time * 100);
}
