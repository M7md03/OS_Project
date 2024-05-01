#include "headers.h"

// Function to clear resources and handle SIGINT signal
void clearResources(int);

// Function to get the number of processes from a file
int getNoOfProcesses(FILE *);

// Function to skip a line in a file
void skipLine(FILE *);

int msgq_id;

int main(void) {
    signal(SIGINT, clearResources);

    // Generate a unique key for the message queue
    key_t key_id = ftok("keyfile", 65);

    // Create or get the message queue
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    int QUANTA = 0;
    int ALGORITHM;

    int NumberOfProcesses = 0;
    struct Process **Processes;
    int send_val;

    // Open the file containing the processes
    FILE *fptr = fopen("processes.txt", "r");

    if (fptr == NULL) {
        printf("Unable to read the file please locate it and try again.");
        return -1;
    }

    skipLine(fptr);
    NumberOfProcesses = getNoOfProcesses(fptr);
    Processes = (struct Process **)malloc(NumberOfProcesses * sizeof(struct Process *));
    skipLine(fptr);

    int Id, At, Rt, Pr;
    for (int i = 0; i < NumberOfProcesses; i++) {
        Processes[i] = (struct Process *)malloc(sizeof(struct Process));
        fscanf(fptr, "%d\t%d\t%d\t%d", &Id, &At, &Rt, &Pr);
        Processes[i]->ID = Id;
        Processes[i]->ArrivalT = At;
        Processes[i]->RunT = Rt;
        Processes[i]->P = Pr;
    }
    printf("the number of processses is %d \n", NumberOfProcesses);
    fclose(fptr);

    printf("Please enter the required scheduling algorithm \n0 FOR HPF \n1 for SRTN \n2 for RR\n");
    scanf("%d", &ALGORITHM);
    if (ALGORITHM == 2) {
        printf("Enter the quanta for RR \n");
        scanf("%d", &QUANTA);
    }

    // Fork a child process to execute the clock process
    int pid = fork();
    if (pid == -1) {
        perror("failure in forking");
    } else if (pid == 0) {
        // Execute the clock process
        char *args[] = {"./clk.out", NULL};
        if (execv(args[0], args) == -1) {
            perror("failure in execv1");
            exit(EXIT_FAILURE);
        }
    }

    // Fork another child process to execute the scheduler process
    pid = fork();
    if (pid == -1) {
        perror("failure in forking");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Execute the scheduler process
        char Buffer_1[20], Buffer_2[20], Buffer_3[20];

        sprintf(Buffer_1, "%d", ALGORITHM);
        sprintf(Buffer_2, "%d", NumberOfProcesses);
        sprintf(Buffer_3, "%d", QUANTA);
        char *args[] = {"./scheduler.out", Buffer_1, Buffer_2, Buffer_3, NULL};

        if (execv(args[0], args) == -1) {
            perror("failure in execv2");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize the clock
    initClk();

    int Count = 0;
    while (Count < NumberOfProcesses) {
        if (getClk() == Processes[Count]->ArrivalT) {
            printf("The current time is %d\n", getClk());
            struct Process p = *(Processes[Count]);
            send_val = msgsnd(msgq_id, &p, sizeof(p), IPC_NOWAIT);
            if (send_val == -1) {
                perror("Error in send");
            } else {
                Count++;
                printf("I sent the message at time %d\n", getClk());
            }
        }
    }

    // Free allocated memory for processes
    for (int i = 0; i < NumberOfProcesses; i++) {
        DeProcess(Processes[i]);
    }

    int status;

    // Wait for child process to finish
    pid = wait(&status);
    if (WIFEXITED(status)) {
        int msgq_del;
        // Remove the message queue
        msgq_del = msgctl(msgq_id, IPC_RMID, 0);
        destroyClk(true);
        exit(0);
    }
}

// Function to clear resources and handle SIGINT signal
void clearResources(int signum) {
    printf("process generator stopped\n");
    int msgq_del;
    // Remove the message queue
    msgq_del = msgctl(msgq_id, IPC_RMID, 0);
    destroyClk(true);
    exit(0);
}

// Function to get the number of processes from a file
int getNoOfProcesses(FILE *f) {
    int c;
    int count = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    fseek(f, 0, SEEK_SET);
    return count;
}

// Function to skip a line in a file
void skipLine(FILE *f) {
    char ignore[IGNORE_LENGTH];
    fgets(ignore, IGNORE_LENGTH, f);
}