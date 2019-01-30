//
//  proc.h
//  SchedulingAlgorithm
//
//  Created by yi fan on 1/24/19.
//  Copyright © 2019 yi fan. All rights reserved.
//
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef proc_h
#define proc_h
typedef struct process {
    int pid;
    int priority;
    float arrivalTime;
    float runTime;
    float waitTime;
    float responseTime;
    float turnaroundTime;
    float completedTime;
    
} process_t;

typedef struct node node_t;

struct node {
    process_t* process;
    node_t* next;
};

// Using a linkedlist to hold pointers to node_t
typedef struct procList {
    node_t* head;
} procList_t;

/*
 More Specific Actions
 */

// Add "proc" at last
node_t* procList_pushLast_helper(node_t* head, process_t* process) {
    if (head == NULL) {
        head = malloc(sizeof(node_t));
        head->process = process;
        head->next = NULL;
    }else if (head->next == NULL) {
        head->next = malloc(sizeof(node_t));
        head->next->process = process;
        head->next->next = NULL;
    } else {
        head->next = procList_pushLast_helper(head->next, process);
    }
    return head;
}

// helper function to serve popBeg function
node_t* procList_popBeg_helper(node_t* head) {
    if (head == NULL) {
        return NULL;
    }else {
        node_t* deletedNode = head;
        head = head->next;
        free(deletedNode);
        deletedNode = NULL;
        return head;
    }
}

void proc_print(process_t* process) {
    printf("pid %d:\t expected runtime=%f,\t arrival=%f,\t completed time=%f,\t remaining time=%f,\t priority=%d\n",
           process->pid, process->runTime, process->arrivalTime, process->completedTime,
           process->runTime - process->completedTime, process->priority);
}

void swap_pr(process_t* pr, int i, int j) {
    process_t* temp = malloc(sizeof(process_t));
    memcpy((void *) temp, &pr[i], sizeof(process_t));
    memcpy(&pr[i], &pr[j], sizeof(process_t));
    memcpy(&pr[j], (void *) temp, sizeof(process_t));
    free(temp);
}

// Quicksort process based on arrival time
void quicksort_arrivalTime(process_t* pr, int low, int high) {
    int left = low;
    int right = high;
    int pivot = high;
    
    if (low >= high) return;
    
    while(1) {
        while(pr[right].arrivalTime >= pr[pivot].arrivalTime && right > 0) right--;
        while(pr[left].arrivalTime < pr[pivot].arrivalTime && left < high) left++;
        if (left >= right) {
            swap_pr(pr, left, pivot);
            break;
        }
        else {
            swap_pr(pr, left, right);
        }
    }
    
    quicksort_arrivalTime(pr, low, left-1);
    quicksort_arrivalTime(pr, left+1, high);
}

process_t* gen_proc_arr(int num) {
    static bool rand_seeded = false;
    if(!rand_seeded) {
        srand(time(NULL)); // enable seed RNG when finish the process of testing
        rand_seeded = true;
    }
    process_t* procs = malloc(sizeof(process_t) * num);
    
    for (int i = 0; i < num; i++) {
        process_t* process = procs + i;
        process->pid = i;
        process->arrivalTime = (float) (rand() % 1000)/10;
        // ^ Float between 0 and 99 that has 1 decimal
        process->runTime = (float) (rand() % 100 + 1) / 10;
        // ^ Float between 0.1 and 10 that has 1 decimal
        process->completedTime = 0.0;
        process->waitTime = 0.0;
        process->turnaroundTime = 0.0;
        process->responseTime = 0.0;
        process->priority = rand() % 4 + 1;
    }
    
    quicksort_arrivalTime(procs, 0, num-1);
    
    return procs;
}
#endif /* proc_h */
