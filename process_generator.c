#include "headers.h"
void clearResources(int);
int getNoOfProcesses(FILE *);
void skipLine(FILE *);
int msgq_id;

int main(void) {
    signal(SIGINT, clearResources);

    key_t key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    int QUANTA = 0;
    int ALGORITHM;

    int NumberOfProcesses = 0;
    struct Process **Processes;
    int send_val;
    FILE *fptr = fopen("processes.txt", "r");

    if (fptr == NULL)  // check if the file opened sucessfully or not
    {
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
    fclose(fptr);  // close the file after reading
    // here we have an array of Inputprocesses
    //  2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("Please enter the required scheduling algorithm \n0 FOR HPF \n1 for SRTN \n2 for RR\n");
    scanf("%d", &ALGORITHM);  // read the input
    if (ALGORITHM == 2) {
        printf("Enter the quanta for RR \n");
        scanf("%d", &QUANTA);
    }

    // 3. Initiate and create the scheduler and clock processes.
    // CLOCK
    int pid = fork();
    if (pid == -1) {
        perror("failure in forking");
    } else if (pid == 0) {
        char *args[] = {"./clk.out", NULL};
        if (execv(args[0], args) == -1) {
            perror("failure in execv1");
            exit(EXIT_FAILURE);
        }
    }
    // SCHEDULER
    pid = fork();
    if (pid == -1) {
        perror("failure in forking");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
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
    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    printf("The clock has been initialized\n");
    // TODO Generation Main Loop

    // 5. Create a data structure for processes and provide it with its parameters.
    int Count = 0;
    while (Count < NumberOfProcesses) {
        if (getClk() == Processes[Count]->ArrivalT) {
            printf("The current time is %d\n", getClk());
            struct Process p = *(Processes[Count]);
            send_val = msgsnd(msgq_id, &p, sizeof(p), IPC_NOWAIT);
            if (send_val == -1)  // check send was sucessful
            {
                perror("Error in send");
            } else {
                Count++;
                printf("I sent the message at time %d\n", getClk());  // print the time of sending
            }
        }
    }
    for (int i = 0; i < NumberOfProcesses; i++) {
        DeProcess(Processes[i]);  // free the processes from memory
    }
    // 7. Clear clock resources
    int status;

    pid = wait(&status);
    if (WIFEXITED(status)) {
        int msgq_del;
        msgq_del = msgctl(msgq_id, IPC_RMID, 0);  // destroy message queue used to communicate with scheduler
        destroyClk(true);                         // destroy the clk
        exit(0);
    }
}

void clearResources(int signum) {
    // TODO Clears all resources in case of interruption
    printf("process generator stopped\n");
    int msgq_del;
    msgq_del = msgctl(msgq_id, IPC_RMID, 0);  // destroy message queue used to communicate with scheduler
    destroyClk(true);                         // destroy the clk
    exit(0);
}

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
void skipLine(FILE *f) {
    char ignore[IGNORE_LENGTH];
    fgets(ignore, IGNORE_LENGTH, f);
}