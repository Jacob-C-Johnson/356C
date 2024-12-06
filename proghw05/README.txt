# Process Scheduler Simulator

This program simulates two CPU scheduling algorithms:
1. First-Come First-Served (FCFS)
2. Preemptive Priority (PP)

## Input Format

The program reads from input.txt with the following format:
    pid arrival_time cpu_time [only for PP]priority

# Compile:
make

# Run FCFS:
./scheduler FCFS < input.txt

# Run PP:
./scheduler PP < input.txt

Example input.txt:
    1 0 3 1
    2 2 4 2
    3 4 2 3
    0 0 0 0

Example output for FCFS:
    Simulation starting:
    0 1 arriving
    0 2 arriving
    0 1 running
    3 3 arriving
    7 4 arriving
    12 1 finished
    12 2 running
    21 2 finished
    21 3 running
    27 3 finished
    27 4 running
    31 4 finished
    Average waiting time: 12.50
    Average response time: 12.50
    Average turnaround time: 20.25
    Average CPU usage: 100.00%

Example output for PP:
Simulation starting:
0 1 arriving
0 2 arriving
0 1 running
1 1 running
2 1 running
3 3 arriving
3 3 running
4 3 running
5 3 running
6 3 running
7 4 arriving
7 4 running
8 2 aging
8 4 running
9 4 running
10 4 running
11 1 aging
11 4 finished
11 2 running
12 2 running
13 2 running
14 2 running
15 3 aging
15 3 running
16 3 running
17 3 finished
17 1 running
18 1 running
19 1 running
20 1 running
21 1 running
22 1 running
23 2 aging
23 2 running
24 2 running
25 2 running
26 2 running
27 2 running
28 2 finished
28 1 running
29 1 running
30 1 running
31 1 finished
Average waiting time: 11.50
Average response time: 2.75
Average turnaround time: 19.25
Average CPU usage: 100.00%

# Notes

- Preemptive Priority increases a processes priority after it has been in the waiting queue for 8 seconds
- FCFS does not print out the entire simulation in order to keep the terminal tidy. PP needs the simulation printed to show priority and aging

# FCFS Notes
- To clean terminal outputs. This version only outputs when events happen.

# Preemptive Priority Notes
- Priority ties are handled by time spent waiting. The example given demonstrates this.

Simulation starting:
0 1 arriving
0 2 arriving
0 1 running
1 1 running
2 1 running
3 3 arriving
3 3 running
4 3 running
5 3 running
6 3 running
7 4 arriving
7 4 running
8 2 aging     # 2 ages up to priority 6 tieing it with process 1
8 4 running
9 4 running
10 4 running
11 1 aging   
11 4 finished
11 2 running  # process 2 runs because it has been waiting longer than 1
12 2 running
13 2 running
14 2 running
15 3 aging
15 3 running
16 3 running
17 3 finished
17 1 running  # process 1 runs because it has been waiting longer than 2
18 1 running
19 1 running
20 1 running
21 1 running
22 1 running
23 2 aging
23 2 running
24 2 running
25 2 running
26 2 running
27 2 running
28 2 finished
28 1 running
29 1 running
30 1 running
31 1 finished