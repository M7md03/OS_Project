#include "headers.h"

void clearResources(int);
int msgq_id;

int main(void)
{
    //signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
     int QUANTA = 0;
     int ALGORITHM;
     struct msgbuff Msg_Snd ;
     int NumberOfProcesses = 0;                     //index of process in the processes array 
     struct Process Processes [10];     //ARRAY OF PROCESSES 
     int send_val;
     char line[MaxLengthOfLine];
     char * argv[100];
         
     //reading input file
     FILE * fptr;
     fptr = fopen("processes.txt","r");      //opening the file to be read "r" is the mode used for reading
     
     if(fptr ==NULL)
     {
        printf("Unable to read the file please locate it and try again.");
        return -1;
     }
    
    //the structure is 4 numbers seperated by tabs so max length is 12 characters to be safe use 30                        
    fgets(line,sizeof(line),fptr);          //read the first line which is a comment
    int Id, At, Rt, Pr;             //place holders for the process details
    //read the numbers in ther correct formatting
    
    while (fscanf(fptr, "%d\t%d\t%d\t%d", &Id, &At, &Rt, &Pr) == 4) {
        // allocate the numbers to the process 
        Processes[NumberOfProcesses].Id = Id;
        Processes[NumberOfProcesses].Arrival_Time = At;
        Processes[NumberOfProcesses].Runtime = Rt;
        Processes[NumberOfProcesses].Priority = Pr;
        NumberOfProcesses++;
    }
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("Please enter the required scheduling algorithm (HPF, SRTN , RR)\n");
    scanf("%d", &ALGORITHM);                //read the input
    if(ALGORITHM == 2)
    {
        printf("Enter the quanta for RR \n");
        scanf("%d",&QUANTA);
    }

    // 3. Initiate and create the scheduler and clock processes.
    // CLOCK
    int pid = fork();
    if (pid == -1)
    {
        perror("failure in forking");
    }
   else if (pid == 0)
    {
        if (execvp("clk.out", argv) == -1)
        {
            perror("failure in execv1");
            exit(EXIT_FAILURE);
        }
        exit(-1);
    }
     // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // SCHEDULER
    pid = fork();
    if (pid == -1)
    {
        perror("failure in forking");
        exit(EXIT_FAILURE);
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
        if (execvp("scheduler.out",argv) == -1)
        {
            perror("failure in execv2");
            exit(EXIT_FAILURE);
        }
    }
    
   
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop


    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    int Count = 0;
    struct Process *SameArrival [10];
        int num = 0;
    while (1)
    {
        num = 0;
        while ((int)getClk() == (int)Processes[Count].Arrival_Time)     //p[0].at=1,p[1].at=1
        {
            //place the processes of the same AT in an array
            SameArrival[num] = &Processes[Count];   
            num++;Count++;
        }
        if(num > 0)
        {
            Msg_Snd.p = SameArrival;   
            Msg_Snd.size = num;            
            // switch (ALGORITHM)
            // {
            // case SRTN:
            //     Msg_Snd.Process.Remaining_Time = Msg_Snd.Process.Input.Runtime;

            //     Msg_Snd.Process.Input.Priority = Msg_Snd.Process.Input.Runtime;
            //     break;
            // case RR:
            //     Msg_Snd.Process.Remaining_Time = Msg_Snd.Process.Input.Runtime;
            //     Msg_Snd.Process.Input.Priority = 1;

            //     break;

            // default:
            //     break;
            //}
            //Msg_Snd.m_type = Msg_Snd.Process.Input.Id;
            send_val = msgsnd(msgq_id, &Msg_Snd, sizeof(Msg_Snd.p), !IPC_NOWAIT);
            if (send_val == -1)
            {
                perror("Error in send");
            }
            if (Count == NumberOfProcesses)
            {
                break;
            }
        }
        //increment the clock
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
    //free(Processes);
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
