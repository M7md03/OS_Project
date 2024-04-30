#include "headers.h"
void clearResources(int);
int msgq_id;

int main(void) {
    // initializing MessageQueue
    key_t key_id = ftok("keyfile", 65);          // CREATE A UNIQUE KEY
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);  // CREATE MESSAGE QUEUE THEN RETURN THE ID
    // signal(SIGINT, clearResources);
    //  TODO Initialization
    //  1. Read the input files.
    int QUANTA = 0;
    int ALGORITHM;
    struct msgbuff Msg_Snd;
    int NumberOfProcesses = 0;                   // index of process in the processes array
    struct Process *Processes[No_of_Processes];  // ARRAY OF PROCESSES
    int send_val;
    char line[MaxLengthOfLine];
    FILE *fptr;  // pointer to file

    fptr = fopen("processes.txt", "r");  // opening the file to be read "r" is the mode used for reading

    if (fptr == NULL)  // check if the file opened sucessfully or not
    {
        printf("Unable to read the file please locate it and try again.");
        return -1;
    }

    fgets(line, sizeof(line), fptr);  // read the first line which is a comment
    int Id, At, Rt, Pr;               // place holders for the process details

    while (fscanf(fptr, "%d\t%d\t%d\t%d", &Id, &At, &Rt, &Pr) == 4)  // read the numbers in ther correct formatting
    {
        Processes[NumberOfProcesses] = Process(Id, At, Rt, Pr);  // create a process using the values
        NumberOfProcesses++;
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
    // TODO Generation Main Loop

    // 5. Create a data structure for processes and provide it with its parameters.
    int Count = 0, x;
    while (1) {
        x = getClk();
        if (x < Processes[Count]->ArrivalT) {
            continue;
        }
        // 6. Send the information to the scheduler at the appropriate time.
        while (x == Processes[Count]->ArrivalT)  // check arrival time of process with the clock
        {
            printf("The current time is %d\n", x);
            Msg_Snd.p = *(Processes[Count]);  // set the process in message buffer
            send_val = msgsnd(msgq_id, &Msg_Snd, sizeof(Msg_Snd.p), IPC_NOWAIT);
            if (send_val == -1)  // check send was sucessful
            {
                perror("Error in send");
            }
            Count++;
            printf("I sent the message at time %d\n", x);  // print the time of sending
        }
        if (Count == NumberOfProcesses) {
            break;
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
