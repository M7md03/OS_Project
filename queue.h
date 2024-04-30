//  Priority Queue
#include <stdio.h>
#include <limits.h>
#include<math.h>
#define MaxLengthOfLine 100
struct Pair
{
    int start;
    int end;
};
struct Process
{
  int Id;
  int Arrival_Time;
  int Runtime;
  int Priority;
  //int memorysize;
};
struct Node
{
  struct Process Input;       //process node
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
  struct Process **p;
  int size;
};
