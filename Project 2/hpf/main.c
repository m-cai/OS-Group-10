//
//  main.c
//  SchedulingAlgorithm
//
//  Created by yi fan on 1/24/19.
//  Copyright © 2019 yi fan. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "proc.h"
#include "srtf.h"
#include "hpf.h"

void setAlgoPerformanceOf(int algoIndex,float *inputArray);
void printAlgoPerformanceOf(int algoIndex);

int main(int argc, const char * argv[]) {
    int n, type;
    printf("How many processes do you want:");  // Input number of processes
    scanf("%d", &n);
    printf("\nPlease select a scheduler:n1.SRT\n2.HPF(Non preemtive)\n3.HPF(Preemtive)\n");
    scanf("%d", &type);
    switch (type) {
        case 1: printf("SRT");
                executeSRTF(n);
                break;
        case 2: printf("HPF(Non preemtive)");
                HPF_NONPRE(n);
                break;
        case 3: printf("HPF(Preemtive)");
                HPF_PREEMP(n);
                break;
    }
}
