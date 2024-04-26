
#include "RR.h"

int main() {
    struct RoundRobin *rr = RoundRobin(2);

    // Example usage:
    struct Process *p1 = Process(1, 0, 5, 2);
    struct Process *p2 = Process(2, 2, 3, 1);
    struct Process *p3 = Process(3, 4, 4, 3);

    enqueue(rr, p1);
    enqueue(rr, p2);
    enqueue(rr, p3);

    struct Process *p;
    while (!isEmpty(rr)) {
        p = dequeue(rr);
        if (p->RemT <= rr->quantum) {
            printf("Process with ID %d finished\n", p->ID);
            p->RemT = 0;
        } else {
            p->RemT -= rr->quantum;
            printf("Process with ID %d processed for quantum, Remaining Time = %d\n", p->ID, p->RemT);
            enqueue(rr, p);
        }
    }

    FreeRoundRobin(rr);

    return 0;
}