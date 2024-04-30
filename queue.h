#ifndef QUEUE_H
#define QUEUE_H
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#define MaxLengthOfLine 100
#define No_of_Processes 50
//  Priority Queue
struct Pair {
    int start;
    int end;
};
struct Process {
    pid_t pid;  // Process ID
    int ID;
    int ArrivalT;  // Arrival Time
    int StartT;    // Start Time
    int RunT;      // Running Time
    int RemT;      // Remaining Time
    int EndT;      // End Time
    int P;         // Priority
    // int MemorySize;     //for phase 2
};
// struct Node
// {
//   //struct InputProcess Input;       //process node
//   int Start_Time;
//   int Constant_Start_Time;
//   int Finish_Time;
//   int Remaining_Time;
//   int Priority;
//   int status;
//   int pid;
//   struct Pair Allocation;
// };
struct msgbuff {
    struct Process p;
    int flag;
};
#endif
