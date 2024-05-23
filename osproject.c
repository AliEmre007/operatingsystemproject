
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

void read_processes(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Could not open input file"); // Print error message if the file could not be opened.
        exit(EXIT_FAILURE);                  // Exit the program with failure status.
    }

    char line[MAX_ROW_LENGTH]; // Buffer to store each line read from the file.
    while (fgets(line, sizeof(line), file))
    {                                                                                                            // Read each line from the file.
        Process p;                                                                                               // Create a temporary Process variable to hold the data.
        sscanf(line, "%[^,],%d,%d,%d,%d,%d", p.id, &p.arrival_time, &p.priority, &p.burst_time, &p.ram, &p.cpu); // Parse the line and store the values in the Process variable.
        processes[process_tracker++] = p;                                                                        // Add the Process to the array and increment the process count.
    }

    fclose(file); // Close the input file.
}

void fileLogging(const char *message)
{                                          // Function to log a message to a file.
    FILE *file = fopen("output.txt", "a"); // Open the output file in append mode.
    if (!file)
    {                                            // Check if the file was opened successfully.
        perror("opening error for output file"); // Print error message if the file could not be opened.
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s\n", message); // Write the message to the file, followed by a newline.
    fclose(file);                   // Close the output file.
}

void log_queues()
{                                   // Function to log the current state of process queues.
    char logAllert[MAX_ROW_LENGTH]; // Buffer to store the log message.

    strcpy(logAllert, "CPU-1 que1 (priority-0) (firstComeFirstServe) → "); // Initialize the log message for priority 0 processes.
    for (int i = 0; i < process_tracker; i++)
    {
        if (processes[i].priority == 0)
        {
            strcat(logAllert, processes[i].id);
            strcat(logAllert, "-");
        }
    }
    logAllert[strlen(logAllert) - 1] = '\0';
    printf("%s\n", logAllert);

    strcpy(logAllert, "CPU-2 que2 (priority-1) (shortestJobFirst) → "); // Initialize the log message for priority 1 processes.
    for (int i = 0; i < process_tracker; i++)
    {
        if (processes[i].priority == 1)
        {                                       // Check if the process has priority 1.
            strcat(logAllert, processes[i].id); // Append the process ID to the log message.
            strcat(logAllert, "-");             // Append a dash to separate process IDs.
        }
    }
    logAllert[strlen(logAllert) - 1] = '\0';
    printf("%s\n", logAllert);

    strcpy(logAllert, "CPU-2 que3 (priority-2) (RR-q8) → "); // Initialize the log message for priority 2 processes.
    for (int i = 0; i < process_tracker; i++)
    {
        if (processes[i].priority == 2)
        {                                       // Check if the process has priority 2.
            strcat(logAllert, processes[i].id); // Append the process ID to the log message.
            strcat(logAllert, "-");             // Append a dash to separate process IDs.
        }
    }
    logAllert[strlen(logAllert) - 1] = '\0';
    printf("%s\n", logAllert);

    strcpy(logAllert, "CPU-2 que4 (priority-3) (RR-q16) → "); // Initialize the log message for priority 3 processes.
    for (int i = 0; i < process_tracker; i++)
    {
        if (processes[i].priority == 3)
        { // Check if the process has priority 3.
            strcat(logAllert, processes[i].id);
            strcat(logAllert, "-");
        }
    }
    logAllert[strlen(logAllert) - 1] = '\0';
    printf("%s\n", logAllert);
}

int resourceCheck(int priority, int required_ram, int required_cpu)
{ // Function to check if resources are available for a process.
    if (priority == 0)
    {
        return (required_ram <= available_ram_priority_0 && required_cpu <= available_cpu); // Return true if both RAM and CPU requirements can be met for priority 0.
    }
    else
    {
        return (required_ram <= available_ram_other && required_cpu <= available_cpu); // Return true if both RAM and CPU requirements can be met for non-priority 0.
    }
}

void resourceAllocation(int priority, int required_ram, int required_cpu)
{ // Function to allocate resources to a process.
    if (priority == 0)
    {
        available_ram_priority_0 -= required_ram; // Deduct the required RAM from the available RAM for priority 0.
    }
    else
    {
        available_ram_other -= required_ram;
    }
    available_cpu -= required_cpu;
}

void resourceReleasing(int priority, int released_ram, int released_cpu)
{ // Function to release resources back to the pool after a process finishes.
    if (priority == 0)
    {
        available_ram_priority_0 += released_ram; // Add the released RAM back to the available RAM for priority 0.
    }
    else
    {
        available_ram_other += released_ram; // Add the released RAM back to the available RAM for non-priority 0.
    }
    available_cpu += released_cpu; // Add the released CPU back to the available CPU.
}
Process *sortByBurstTime(Process *queue, int count)
{ // Function to sort processes by their burst time using bubble sort.
    Process *sorted = (Process *)malloc(count * sizeof(Process));
    memcpy(sorted, queue, count * sizeof(Process)); // Copy the original queue into the sorted array.
    for (int i = 0; i < count - 1; i++)
    { // Outer loop for bubble sort, iterates over the entire array.
        for (int j = 0; j < count - i - 1; j++)
        { // Inner loop for bubble sort, performs the comparison and swapping.
            if (sorted[j].burst_time > sorted[j + 1].burst_time)
            { // Swap operations
                Process temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    return sorted;
}
