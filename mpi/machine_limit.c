/*
    This program give us the precision limits of our system
*/


#include <stdio.h>
#include <float.h>

int main() {
    printf("Significant Digits: %d\n", LDBL_DIG);
    printf("Max Value: %Le\n", LDBL_MAX);
    return 0;
}
