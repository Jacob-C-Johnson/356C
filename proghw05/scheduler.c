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
    int last_update_time; // For aging calculations
} Process;

// Function prototypes
void pp_scheduler(queue process_queue, Process* process_list[], int num_processes);
void print_summary(Process* processes[], int num_processes, int total_time, int cpu_time);
void age_processes(queue ready_queue, int current_time);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [PP]\n", argv[0]);
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
        process->last_update_time = arrival_time;

        enqueue(process_queue, process);
        process_list[num_processes++] = process;
    }

    if (strcmp(algorithm, "PP") == 0) {
        pp_scheduler(process_queue, process_list, num_processes);
    } else {
        fprintf(stderr, "Invalid algorithm: %s\n", algorithm);
        return 1;
    }

    return 0;
}

void pp_scheduler(queue process_queue, Process* process_list[], int num_processes) {
    int current_time = 0, cpu_time = 0;
    Process* running_process = NULL;
    queue ready_queue = newqueue();

    while (!isempty(process_queue) || running_process || !isempty(ready_queue)) {
        // Check for process arrivals
        while (!isempty(process_queue)) {
            Process* process = (Process*)peek(process_queue);
            if (process->arrival_time > current_time) break;
            dequeue(process_queue);
            enqueue(ready_queue, process);
            printf("%d\t%d\tarriving\n", current_time, process->pid);
        }

        // Aging: Update priorities of processes in the ready queue
        if (current_time % 8 == 0 && !isempty(ready_queue)) {
            queue temp_queue = newqueue();
            while (!isempty(ready_queue)) {
                Process* process = (Process*)dequeue(ready_queue);
                if (current_time - process->last_update_time >= 8) {
                    process->priority++;
                    process->last_update_time = current_time;
                    printf("%d\t%d\taging\n", current_time, process->pid);
                }
                enqueue(temp_queue, process);
            }
            while (!isempty(temp_queue)) {
                enqueue(ready_queue, dequeue(temp_queue));
            }
        }

        // Check if running process has finished
        if (running_process && running_process->remaining_time == 0) {
            printf("%d\t%d\tfinished\n", current_time, running_process->pid);
            running_process->completion_time = current_time;
            running_process->turnaround_time = running_process->completion_time - running_process->arrival_time;
            running_process->waiting_time = running_process->turnaround_time - running_process->cpu_time;
            running_process = NULL;
        }

        // Preemptive priority: Select the highest-priority process
        if (!running_process || (!isempty(ready_queue) && ((Process*)peek(ready_queue))->priority > running_process->priority)) {
            if (running_process) {
                enqueue(ready_queue, running_process);
            }
            running_process = (Process*)dequeue(ready_queue);
            if (running_process->response_time == -1) {
                running_process->response_time = current_time - running_process->arrival_time;
            }
            printf("%d\t%d\trunning\n", current_time, running_process->pid);
        }

        // Advance the clock and process running tasks
        current_time++;
        if (running_process) {
            running_process->remaining_time--;
            cpu_time++;
        } else {
            printf("%d\tCPU idle\n", current_time);
        }
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
