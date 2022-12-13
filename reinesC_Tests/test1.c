#include <stdio.h>
#include <stdlib.h>

void getOpticalDistance(char* optDist);

int distance = 59;

int main() {
    char str[16];
    getOpticalDistance(str);
    for(int i = 0; i < 16; i++) {
        printf("%c", str[i]);
    }
    int a = 400;
    char dig[5];
    if (a >= 401) {
        dig[0] = '>';
    } else {
        dig[0] = ' ';
    }
    sprintf(&dig[1], "%0.3d", -10);
    printf("%s", dig);

    return 0;
}

void getOpticalDistance(char* optDist) {
    char steps = (distance/28)+1;
    optDist[0] = '|';
    for(int i = 1; i < steps; i++) {
        optDist[i] = '-';
    }
    
    for(int i = steps; i < 15; i++) {
        optDist[i] = '_';
    }
    optDist[15] = '|';
}