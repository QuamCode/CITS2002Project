#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//  you may need other standard header files


//  CITS2002 Project 1 2023
//  Student1:   23326698    Jack Blackwood   
//  Student2:   23476614    Emily Matthews


//  myscheduler (v1.0)
//  Compile with:  cc -std=c11 -Wall -Werror -o myscheduler myscheduler.c


//  THESE CONSTANTS DEFINE THE MAXIMUM SIZE OF sysconfig AND command DETAILS
//  THAT YOUR PROGRAM NEEDS TO SUPPORT.  YOU'LL REQUIRE THESE //  CONSTANTS
//  WHEN DEFINING THE MAXIMUM SIZES OF ANY REQUIRED DATA STRUCTURES.

#define MAX_DEVICES                     4
#define MAX_DEVICE_NAME                 20
#define MAX_COMMANDS                    10
#define MAX_COMMAND_NAME                20
#define MAX_SYSCALLS_PER_PROCESS        40
#define MAX_RUNNING_PROCESSES           50

//  NOTE THAT DEVICE DATA-TRANSFER-RATES ARE MEASURED IN BYTES/SECOND,
//  THAT ALL TIMES ARE MEASURED IN MICROSECONDS (usecs),
//  AND THAT THE TOTAL-PROCESS-COMPLETION-TIME WILL NOT EXCEED 2000 SECONDS
//  (SO YOU CAN SAFELY USE 'STANDARD' 32-BIT ints TO STORE TIMES).

#define DEFAULT_TIME_QUANTUM            100

#define TIME_CONTEXT_SWITCH             5
#define TIME_CORE_STATE_TRANSITIONS     10
#define TIME_ACQUIRE_BUS                20

// DEFINE THE 3 PROCESS QUEUES
//#define READY_QUEUE                     

//  ----------------------------------------------------------------------

#define CHAR_COMMENT                    '#'

#define MAX_QUEUE_SIZE                  MAX_RUNNING_PROCESSES




//Create a struct for the device info
typedef struct {
    char devicename[MAX_DEVICE_NAME];
    long long readspeed;
    long long writespeed;
} DeviceInfo;
//Create a struct for the device storage
typedef struct {
    DeviceInfo devices[MAX_DEVICES];
    int num_devices;
} DeviceStorage;


//Create a struct for the syscall
typedef struct {
    char syscall[MAX_COMMAND_NAME];
    int elapsed_time;
    char arg1[MAX_COMMAND_NAME];
    char arg2[MAX_COMMAND_NAME];
} Syscall;

//Create a struct for the command
typedef struct {
    char name[MAX_COMMAND_NAME];
    Syscall syscalls[MAX_SYSCALLS_PER_PROCESS];
    int num_syscalls;
} Command;

//Create a struct for the command storage
typedef struct {
    Command commands[MAX_COMMANDS];
    int num_commands;
} CommandStorage;


//  ----------------------------------------------------------------------
//  CPU EXECUTION QUEUE

//  Define the states
typedef enum {READY, RUNNING, BLOCKEDm, EXIT} State;

//Create a struct for the process
typedef struct {
    Command command;
    State state;
    int id;
} Process;

//Create a struct for the queue
typedef struct {
    Process processes[MAX_RUNNING_PROCESSES];
    int num_processes;
    int front;
    int rear;
} Queue;

Queue readyQueue, runningQueue, blockedQueue, exitQueue;
//  ----------------------------------------------------------------------

void initializeQueue(Queue *queue) {
    queue->num_processes = 0;
    queue->front = 0;
    queue->rear = -1;
}

int isEmpty(Queue *queue) {
    return queue->num_processes == 0;
}

// Check if the queue is full
int isFull(Queue *queue) {
    return queue->num_processes == MAX_QUEUE_SIZE;
}


void enqueue(Queue *queue, Process process) {
    if(isFull(queue)) {
        printf("Error: Queue is full\n");
    } else {
        queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
        queue->processes[queue->rear] = process;
        queue->num_processes++;
    }

}

Process dequeue(Queue *queue) {
    if(isEmpty(queue)) {
        printf("Error: Queue is empty\n");
        Process process;
        return process;
    } else {
        Process process = queue->processes[queue->front];
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
        queue->num_processes--;
        return process;
    }
}

void executeSyscall(Process *process, DeviceStorage *deviceStorage) {
    for (int i = 0; i < process->command.num_syscalls; i++) {
            Syscall syscall = process->command.syscalls[i];
            printf("Executing syscall %s\n", syscall.syscall);
            //Check if the syscall is a sleep, read, write, spawn or exit
            if (strcmp(syscall.syscall, "sleep") == 0) {
                //Sleep for the elapsed time
                //syscall.arg1 is the sleep time
            } else if (strcmp(syscall.syscall, "read") == 0) {
                //syscall.arg1 is the device name, syscall.arg2 is the number of bytes to read
                //Check if the device exists
            } else if (strcmp(syscall.syscall, "write") == 0) {
                //syscall.arg1 is the device name, syscall.arg2 is the number of bytes to write
                //Check if the device exists
            } else if (strcmp(syscall.syscall, "spawn") == 0) {
                //syscall.arg1 is the command name
                //Check if the command exists
            } else if (strcmp(syscall.syscall, "wait") == 0) {
                //wait for the process to finish
                //Gotta do some queue stuff
            } else if (strcmp(syscall.syscall, "exit") == 0) {
                //Move the process to the exit queue
                process->state = EXIT;
                Process process = dequeue(&runningQueue);
                enqueue(&exitQueue, process);
                //dequeue from running queue
                printf("Moving Process ID %d with name %s to exit queue\n", process.id, process.command.name);
                // break out of the loop - no more syscalls to execute, even though it should technically be the last syscall anyway
                break;
            }
            }
}

void execute_commands(CommandStorage *commandStorage, DeviceStorage *deviceStorage)
{

    //Create a queue for each state


    //Initialize the queues
    initializeQueue(&readyQueue);
    initializeQueue(&runningQueue);
    initializeQueue(&blockedQueue);
    initializeQueue(&exitQueue);

    //Create an initial process for each command add it to the ready queue
    for (int i = 0; i < commandStorage->num_commands; i++) {
        Process process;
        process.command = commandStorage->commands[i];
        process.state = READY;
        process.id = i;
        enqueue(&readyQueue, process);
        printf("Adding Process ID %d with name %s to ready queue\n", i, process.command.name);
    }

    // While there are still processes in the ready queue execute them and move them to the running queue

    while (!isEmpty(&readyQueue)) {
        Process process = dequeue(&readyQueue);
        process.state = RUNNING;
        enqueue(&runningQueue, process);
        printf("Moving Process ID %d with name %s to running queue\n", process.id, process.command.name);
        //Execute the process syscalls
        executeSyscall(&process, deviceStorage);
        }
    }


void initializeCommandStorage(CommandStorage *storage) {
    storage->num_commands = 0;
}

void initializeDeviceStorage(DeviceStorage *storage) {
    storage->num_devices = 0;
}


void addCommand(CommandStorage *storage, Command command) {
    if (storage->num_commands < MAX_COMMANDS) {
        storage->commands[storage->num_commands] = command;
        storage->num_commands++;
    } else {
        printf("Error: Maximum number of commands reached.\n");
    }
}

void addDevice(DeviceStorage *storage, DeviceInfo device) {
    if (storage->num_devices < MAX_DEVICES) {
        storage->devices[storage->num_devices] = device;
        storage->num_devices++;
    } else {
        printf("Error: Maximum number of devices reached.\n");
    }
}




void read_sysconfig(char argv0[], char filename[], DeviceStorage *deviceStorage)
{

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char buffer[9999];

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        if (buffer[0] == CHAR_COMMENT || buffer[0] == '\n') {
            // Ignore comments and empty lines
            continue;
        }
        char token[100];

        if (sscanf(buffer, "%s", token) == 1) {
            if (strcmp(token, "device") == 0) {
                char devicename[MAX_DEVICE_NAME];
                long long readspeed;
                long long writespeed;
                // out of buffer find first string and ignore, then find devicename string store it, then find readspeed and writespeed as long long and store
                if(sscanf(buffer, "%*s %s %lldBps %lldBps", devicename, &readspeed, &writespeed) == 3){
                    DeviceInfo device;
                    strcpy(device.devicename, devicename);
                    device.readspeed = readspeed;
                    device.writespeed = writespeed;
                    addDevice(deviceStorage, device);
                }
                //No idea what this is for
            } else if (strcmp(token, "timequantum") == 0) {
                int timequantum;
                if (sscanf(buffer, "%*s %d", &timequantum) == 1) {
                    printf("Time Quantum: %dusec\n", timequantum);
                }
            }
        }

    }

    fclose(fp);
}


void read_commands(char argv0[], char filename[], CommandStorage *commandStorage)
{
    // open the commands file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("unable to open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char buffer[9999];
    Command current_command;
    current_command.num_syscalls = 0;
    int newCommand = 0;

    while (fgets(buffer, sizeof buffer, fp) != NULL) {
        // ignore comments and new line
        if (buffer[0] == CHAR_COMMENT || buffer[0] == '\n') continue;
        
        if (buffer[0] != '\t'){
            //Only add the previous current command once we come across a new command
            if (newCommand) {
                addCommand(commandStorage, current_command);
                current_command.num_syscalls = 0;
                newCommand = 0;
            }
            // Parse the command name
            sscanf(buffer, "%s", current_command.name);
            newCommand = 1;
        } else {
            Syscall syscall;
            int parsed_fields = sscanf(buffer, " %dusecs %s %s %s", &syscall.elapsed_time, syscall.syscall, syscall.arg1, syscall.arg2);

            if (parsed_fields >= 2) {
                // At least the elapsed time and syscall name were successfully parsed.
                // Check if the arguments are empty and set them to empty strings if necessary.
                if (parsed_fields < 3) {
                    // Argument 1 is empty
                    strcpy(syscall.arg1, "");
                }
                if (parsed_fields < 4) {
                    // Argument 2 is empty
                    strcpy(syscall.arg2, "");
                }

                // Add the syscall to the current command.
                current_command.syscalls[current_command.num_syscalls++] = syscall;
            } else {
                printf("Error parsing line: %s\n", buffer);
            }
            }
    }
    // Add the last command
    if (newCommand) {
        addCommand(commandStorage, current_command);
    }

    // finished with the file
    fclose(fp);
}

//  ----------------------------------------------------------------------


void printCommandStorage(CommandStorage *storage) {
    for (int i = 0; i < storage->num_commands; i++) {
        Command command = storage->commands[i];
        printf("Command %d: %s\n", i, command.name);
        for (int j = 0; j < command.num_syscalls; j++) {
            Syscall syscall = command.syscalls[j];
            printf("    Syscall %d: %dusecs %s %s %s\n", j, syscall.elapsed_time, syscall.syscall, syscall.arg1, syscall.arg2);
        }
    }
}

void printDeviceStorage(DeviceStorage *storage) {
    for (int i = 0; i < storage->num_devices; i++) {
        DeviceInfo device = storage->devices[i];
        printf("Device %d: %s, %lldBps, %lldBps\n", i, device.devicename, device.readspeed, device.writespeed);
    }
}


//  ----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    CommandStorage commandStorage;
    DeviceStorage deviceStorage;
    initializeCommandStorage(&commandStorage);
    initializeDeviceStorage(&deviceStorage);
//  ENSURE THAT WE HAVE THE CORRECT NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1], &deviceStorage);

//  READ THE COMMAND FILE
    read_commands(argv[0], argv[2], &commandStorage);

    printCommandStorage(&commandStorage);
    printDeviceStorage(&deviceStorage);


//  EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
   execute_commands(&commandStorage, &deviceStorage);

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", 0, 0);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4

