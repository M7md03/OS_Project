#include "HPF.h"
#include "RR.h"
#include "headers.h"

int main(int argc, char* argv[]) {
    int Alg = atoi(argv[1]);
    int ProcNum = atoi(argv[2]);
    int Quantum = atoi(argv[3]);

    initClk();
    if (Alg == HPF) {
        HPFscheduling(ProcNum);
    } else if (Alg == SRTN) {
    } else if (Alg == RR) {
        RoundRobinScheduling(Quantum, ProcNum);
    }
    printf("Scheduler Terminated\n");
    destroyClk(false);
}