#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include<math.h>
#define MaxLengthOfLine 100
//  Priority Queue
struct Pair
{
    int start;
    int end;
};
// struct InputProcess        not needed we read the file and create a process directly
// {
//   int Id;
//   int Arrival_Time;
//   int Runtime;
//   int Priority;
//   //int memorysize;
// };
struct Process {
    pid_t pid;     // Process ID
    int ID;
    int ArrivalT;  // Arrival Time
    int StartT;    // Start Time
    int RunT;      // Running Time
    int RemT;      // Remaining Time
    int EndT;      // End Time
    int P;         // Priority
};
struct Node
{
  //struct InputProcess Input;       //process node
  int Start_Time;
  int Constant_Start_Time;
  int Finish_Time;
  int Remaining_Time;
  int Priority;
  int status;
  int pid;
  struct Pair Allocation;
};
struct msgbuff
{
  struct Process p;
};
#endif 
