
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESERVED_RAM_PRIORITY_0 512
#define MAX_PROCESSES 100
#define TOTAL_RAM 2048
#define MAX_ROW_LENGTH 100
#define CPU_LIMIT 100

typedef struct
{
    char id[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram;
    int cpu;
} Process;

Process processes[MAX_PROCESSES]; // Array to store processes.
int process_tracker = 0;          // Counter to keep track of the number of processes read from the file.

int available_ram_priority_0 = RESERVED_RAM_PRIORITY_0;
int available_ram_other = TOTAL_RAM - RESERVED_RAM_PRIORITY_0;
int available_cpu = CPU_LIMIT;