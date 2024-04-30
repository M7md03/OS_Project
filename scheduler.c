#include "RR.h"
#include "headers.h"

int main(int argc, char* argv[]) {
    int Alg = atoi(argv[1]);
    int ProcNum = atoi(argv[2]);
    int Quantum = atoi(argv[3]);
    initClk();
    if (Alg == 0) {
    } else if (Alg == 1) {
    } else if (Alg == 2) {
        RoundRobinScheduling(Quantum, ProcNum);
    }

    destroyClk(false);
}