#include <stdio.h>
#include <stdlib.h>
#include "static_lib.h"

int main() {

    int c;
    double x, dx;
    
    while(scanf("%d", &c)) {
        if (c == 1) {
            scanf("%lf %lf", &x, &dx);
            printf("[1]: Derivative = %lf\n", Derivative1(x, dx));
        }
        else if (c == 2) {
            scanf("%lf %lf", &x, &dx);
            printf("[2]: Derivative = %lf\n", Derivative2(x, dx));
        }
        else {
            printf("ERROR\n");
            break;
        }
    }

    return 0;
}