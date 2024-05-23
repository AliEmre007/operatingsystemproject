
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


void firstComeFirstServe(Process* queue, int count) {            // Function to handle processes using the First-Come, First-Served (firstComeFirstServe) scheduling algorithm.
    for (int i = 0; i < count; i++) {             // Iterate over each process in the queue.
        char logAllert[MAX_ROW_LENGTH];        // Buffer for log messages.

        if (!resourceCheck(queue[i].priority, queue[i].ram, queue[i].cpu)) {      // Check if there are sufficient resources.
            snprintf(logAllert, sizeof(logAllert), 
            "Process %s cannot be assigned to CPU-1 because of insufficient resources.", queue[i].id);  // Prepare a log message indicating insufficient resources.
            fileLogging(logAllert);             // Log the message to the output file.
            printf("%s\n", logAllert);          
            continue;                             
        }

        snprintf(logAllert, sizeof(logAllert), "Process %s is queued to be assigned to CPU-1.", queue[i].id);  // Prepare a log message indicating the process is queued.
        fileLogging(logAllert);        // Log the message to the output file.
        printf("%s\n", logAllert);     

        resourceAllocation(queue[i].priority, queue[i].ram, queue[i].cpu);  // Allocate resources for the current process.

        snprintf(logAllert, sizeof(logAllert), "Process %s is assigned to CPU-1.", queue[i].id);  // Prepare a log message indicating the process is assigned to CPU-1.
        fileLogging(logAllert);        // Log the message to the output file.
        printf("%s\n", logAllert);     

        snprintf(logAllert, sizeof(logAllert), "Process %s is completed and terminated.", queue[i].id);  // Prepare a log message indicating the process is completed and terminated.
        fileLogging(logAllert);         // Log the message to the output file.
        printf("%s\n", logAllert);      

        resourceReleasing(queue[i].priority, queue[i].ram, queue[i].cpu);  // Release resources used by the current process.
    }
}


void roundRobinScheduling(Process* queue, int count, int quantum) {
    int* theRestOfBurstTime = (int*)malloc(count * sizeof(int));     // Allocate memory for an array to store remaining burst times for each process.
    for (int i = 0; i < count; i++) {
        theRestOfBurstTime[i] = queue[i].burst_time;                 // Initialize the remaining burst time for each process to its burst time.
    }

    int time = 0;        
    while (1) {            // Continue looping indefinitely.
        int done = 1;      // Flag to indicate if all processes are done.

        for (int i = 0; i < count; i++) {           
            if (theRestOfBurstTime[i] > 0) {      // Check if the process still has burst time remaining.
                done = 0;              // Set the done flag to 0 indicating that there are still processes to be executed.

                if (!resourceCheck(queue[i].priority, queue[i].ram, queue[i].cpu)) {     // Check if there are enough resources for the current process.
                    char logAllert[MAX_ROW_LENGTH];     // Buffer for creating log messages.
                    snprintf(logAllert, sizeof(logAllert), "Process %s cannot be assigned to CPU-2 because of insufficient resources.", queue[i].id);  // Create a log message 
                    fileLogging(logAllert);          
                    printf("%s\n", logAllert);       
                    continue;                          
                }

                char logAllert[MAX_ROW_LENGTH];       // Buffer for creating log messages.

                snprintf(logAllert, sizeof(logAllert), "Process %s is placed in the que2 queue to be assigned to CPU-2.", queue[i].id);  // Log message for placing the process in the queue.
                fileLogging(logAllert);           
                printf("%s\n", logAllert);        

                resourceAllocation(queue[i].priority, queue[i].ram, queue[i].cpu);  // Allocate the necessary resources for the process.

                snprintf(logAllert, sizeof(logAllert), "Process %s is assigned to CPU-2.", queue[i].id);  // Log message for assigning the process to CPU-2.
                fileLogging(logAllert);              
                printf("%s\n", logAllert);           

                if (theRestOfBurstTime[i] > quantum) {       // Check if the remaining burst time is greater than the quantum.
                    time += quantum;                           // Increment the time by the quantum.
                    theRestOfBurstTime[i] -= quantum;        // Decrement the remaining burst time by the quantum.

                    snprintf(logAllert, sizeof(logAllert), "Process %s ran for %d seconds and is queued again because the process is not completed.", queue[i].id, quantum);  // Log message for process running and queuing again.
                    fileLogging(logAllert);             
                    printf("%s\n", logAllert);          

                    resourceReleasing(queue[i].priority, queue[i].ram, queue[i].cpu);  // Release the resources used by the process.
                } else {
                    time += theRestOfBurstTime[i];        // Increment the time by the remaining burst time.
                    snprintf(logAllert, sizeof(logAllert), "Process %s ran for %d seconds, its operation is completed and terminated.", queue[i].id, theRestOfBurstTime[i]);  // Log message for process completion and termination.
                    fileLogging(logAllert);               
                    printf("%s\n", logAllert);            

                    theRestOfBurstTime[i] = 0;            // Set the remaining burst time to 0 as the process is completed.

                    resourceReleasing(queue[i].priority, queue[i].ram, queue[i].cpu);      // Release the resources used by the process.
                }
            }
        }

        if (done) break;       // If all processes are done, exit the loop.
    }

    free(theRestOfBurstTime);      // Free the memory allocated for the remaining burst time array.
}


void shortestJobFirst(Process* queue, int count) {
    Process* sorted_queue = sortByBurstTime(queue, count);         // Sort the queue by burst time using the sortByBurstTime function.

    for (int i = 0; i < count; i++) {               
        char logAllert[MAX_ROW_LENGTH];           // Buffer for creating log messages.

        if (!resourceCheck(sorted_queue[i].priority, sorted_queue[i].ram, sorted_queue[i].cpu)) {  // Check if there are enough resources for the current process.
            snprintf(logAllert, sizeof(logAllert), "Process %s cannot be assigned to CPU-2 because of insufficient resources.", sorted_queue[i].id);  // Create a log message 
            fileLogging(logAllert);              
            printf("%s\n", logAllert);           
            continue;         
        }

        snprintf(logAllert, sizeof(logAllert), "Process %s is placed in the que1 queue to be assigned to CPU-2.", sorted_queue[i].id);  // Log message for placing the process in the queue.
        fileLogging(logAllert);             
        printf("%s\n", logAllert);          

        resourceAllocation(sorted_queue[i].priority, sorted_queue[i].ram, sorted_queue[i].cpu);  // Allocate the necessary resources for the process.

        snprintf(logAllert, sizeof(logAllert), "Process %s is assigned to CPU-2.", sorted_queue[i].id);  // Log message for assigning the process to CPU-2.
        fileLogging(logAllert);         
        printf("%s\n", logAllert);      

        snprintf(logAllert, sizeof(logAllert), "The operation of process %s is completed and terminated.", sorted_queue[i].id);  // Log message for process completion and termination.
        fileLogging(logAllert);         
        printf("%s\n", logAllert);      

        resourceReleasing(sorted_queue[i].priority, sorted_queue[i].ram, sorted_queue[i].cpu);  // Release the resources used by the process.
    }
    free(sorted_queue);       
}

int main() {
    read_processes("input.txt");           // Read the processes from the input file.

    Process que1[MAX_PROCESSES], que2[MAX_PROCESSES], que3[MAX_PROCESSES], que4[MAX_PROCESSES];  // Create queues for each priority level.
    int count1 = 0, count2 = 0, count3 = 0, count4 = 0;       // Initialize counts for each queue.

    for (int i = 0; i < process_tracker; i++) {        // Iterate over all processes.
        switch (processes[i].priority) {             // Switch based on the priority of each process.
            case 0: que1[count1++] = processes[i]; break;      
            case 1: que2[count2++] = processes[i]; break;      
            case 2: que3[count3++] = processes[i]; break;      
            case 3: que4[count4++] = processes[i]; break;      
        }
    }

    printf("************* OUTPUT OF THE PROGRAM *************\n"); 

    firstComeFirstServe(que1, count1);                // Execute firstComeFirstServe scheduling for que1.
    shortestJobFirst(que2, count2);                 // Execute shortestJobFirst scheduling for que2.
    roundRobinScheduling(que3, count3, 8);      // Execute round robin scheduling for que3 with quantum 8.
    roundRobinScheduling(que4, count4, 16);     // Execute round robin scheduling for que4 with quantum 16.

    printf("**********************\n");  
    log_queues();         
    printf("**********************\n");  

    return 0;         
}
