// #include "HPF.h"
// #include "RR.h"
// #include "SRTN.h"
#include <stdio.h>

int main() {
    // struct RoundRobin *rr = RoundRobin(2);
    // struct MinHeap *srtn = MinHeap();
    // struct MinHeap *hpf = MinHeap();

    // struct Process *p1 = Process(1, 0, 5, 2);
    // struct Process *p2 = Process(2, 2, 3, 1);
    // struct Process *p3 = Process(3, 4, 4, 3);

    // enqueue(rr, p1);
    // enqueue(rr, p2);
    // enqueue(rr, p3);

    // insertProcessSTRN(srtn, p1);
    // insertProcessSTRN(srtn, p2);
    // insertProcessSTRN(srtn, p3);

    // insertProcessHPF(hpf, p1);
    // insertProcessHPF(hpf, p2);
    // insertProcessHPF(hpf, p3);

    // while (srtn->size > 0) {
    //     struct Process *p = extractMinSRTN(srtn);
    //     printf("Process: %d, Remaining Time: %d\n", p->ID, p->RemT);
    //     if (p->RemT > 0) {
    //         p->RemT--;
    //     }
    // }

    // FreeRoundRobin(rr);
    // FreeMin(srtn);
    // FreeMin(hpf);
    int x = 2;
    char quantum_str[10];
    sprintf(quantum_str, "%d", x);
    char *args[] = {"./process.out", quantum_str, NULL};
    printf("%s\n", args[0]);

    return 0;
}