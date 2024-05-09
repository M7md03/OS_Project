#include "HPF.h"
#include "RR.h"
#include "SRTN.h"
#include "headers.h"

void PrintPerf(FILE *fout, float util, float totalWTA, int totalWait, int ProcNum, float *WTA);

int main(int argc, char *argv[]) {
    int Alg = atoi(argv[1]);
    int ProcNum = atoi(argv[2]);
    int Quantum = atoi(argv[3]);

    FILE *fptr = fopen("scheduler.log", "w");
    FILE *fout = fopen("scheduler.perf", "w");

    float totalWTA = 0.0;
    int totalWait = 0;
    int totalUtil = 0;
    float *WTA = (float *)malloc(ProcNum * sizeof(float));

    if (fptr == NULL) {
        printf("Unable to open the log file.");
    }
    if (fout == NULL) {
        printf("Unable to open the perf file.");
    }

    initClk();

    if (Alg == HPF) {
        HPFScheduling(ProcNum, fptr, &totalWTA, &totalWait, &totalUtil, WTA);
    } else if (Alg == SRTN) {
        SRTNScheduling(ProcNum, fptr, &totalWTA, &totalWait, &totalUtil, WTA);
    } else if (Alg == RR) {
        RoundRobinScheduling(Quantum, ProcNum, fptr, &totalWTA, &totalWait, &totalUtil, WTA);
    }

    int clk = getClk() - 1;
    printf("clk = %d\n", clk);
    printf("totalutil = %d\n", totalUtil);
    float util = (float)(((float)(totalUtil + 1.0) / (float)clk) * 100);
    printf("util = %.2f\n", util);
    PrintPerf(fout, util, totalWTA, totalWait, ProcNum, WTA);

    fclose(fptr);
    fclose(fout);
    printf("Scheduler Terminated\n");
    free(WTA);
    destroyClk(false);
    return 0;
}

void PrintPerf(FILE *fout, float util, float totalWTA, int totalWait, int ProcNum, float *WTA) {
    double sum = 0.0;
    fprintf(fout, "CPU utitilization = %.2f%%\n", util);
    fprintf(fout, "Avg WTA = %.2f\n", totalWTA / ProcNum);
    fprintf(fout, "Avg Waiting = %.2f\n", (float)totalWait / ProcNum);
    for (int i = 0; i < ProcNum; i++) {
        sum += (WTA[i] - totalWTA / ProcNum) * (WTA[i] - totalWTA / ProcNum);
    }
    fprintf(fout, "Std WTA = %.2f\n", sqrt(sum / ProcNum));
}
