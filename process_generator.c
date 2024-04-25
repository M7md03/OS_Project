#include "headers.h"

void clearResources(int);
int msgq_id;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
     int QUANTA = 0;
     int ALGORITHM;
     struct msgbuff Msg_Snd ;
     int NumberOfProcesses = 0;
     struct InputVariables Processes [MaxLengthOfLine];
     int send_val;
     char line[MaxLengthOfLine];
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.


    // 3. Initiate and create the scheduler and clock processes.
    // CLOCK
    int pid = fork();
    if (pid == -1)
    {
        perror("failure in forking");
    }
   else if (pid == 0)
    {
        if (execv("./clk.out", argv) == -1)
        {
            perror("failure in execv");
        }
    }
    // SCHEDULER
    pid = fork();
    if (pid == -1)
    {
        perror("failure in forking");
    }
    else if (pid == 0)
    {
        char Buffer_1[20];
        char Buffer_2[20];
        char Buffer_3[20];
        sprintf(Buffer_1, "%d", ALGORITHM);
        sprintf(Buffer_2, "%d", NumberOfProcesses);
        sprintf(Buffer_3, "%d", QUANTA);
        argv[1] = Buffer_1;
        argv[2] = Buffer_2;
        argv[3] = Buffer_3;
        if (execv("./scheduler.out", argv) == -1)
        {
            perror("failure in execv");
        }
    }
    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop


    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    int Count = 0;
    while (1)
    {
        while ((int)getClk() < (int)Processes[Count].Arrival_Time)
        {
        }

        Msg_Snd.Process.Input = Processes[Count];
        switch (ALGORITHM)
        {
        case SRTN:
            Msg_Snd.Process.Remaining_Time = Msg_Snd.Process.Input.Runtime;

            Msg_Snd.Process.Input.Priority = Msg_Snd.Process.Input.Runtime;
            break;
        case RR:
            Msg_Snd.Process.Remaining_Time = Msg_Snd.Process.Input.Runtime;
            Msg_Snd.Process.Input.Priority = 1;

            break;

        default:
            break;
        }
        Msg_Snd.m_type = Msg_Snd.Process.Input.Id;
        send_val = msgsnd(msgq_id, &Msg_Snd, sizeof(Msg_Snd.Process), !IPC_NOWAIT);
        if (send_val == -1)
        {
            perror("Error in send");
        }
        Count++;
        if (Count == NumberOfProcesses)
        {
            break;
        }
    }
    // 7. Clear clock resources
        int status;
        pid = wait(&status);
    if (WIFEXITED(status))
    {
        int msgq_del;
        msgq_del = msgctl(msgq_id, IPC_RMID, 0);
        destroyClk(true);
        exit(0);
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
     printf("process generator stopped\n");
    int msgq_del;
    msgq_del = msgctl(msgq_id, IPC_RMID, 0);
    destroyClk(true);
    exit(0);
}
