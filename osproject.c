
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