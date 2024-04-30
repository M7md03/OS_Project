// #include "HPF.h"
// #include "RR.h"
// #include "SRTN.h"
#include <stdio.h>

int main() {
    int *x = (int *)malloc(sizeof(int));

    free(x);

    if (x == NULL) {
        printf("x is NULL\n");
    }

    return 0;
}