#include "headers.h"

int main(int argc, char* argv[]) {
    int Alg = atoi(argv[1]);
    int Quantum = atoi(argv[2]);
    initClk();

    destroyClk(true);
}