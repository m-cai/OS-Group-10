/*
 * Author: Maggie Cai
 * File: rr.h
 * Description: This header file contains the declarations of functions for the round robin scheduling algorithm.
 */
#ifndef RR_H
#define RR_H

typedef struct process Process;
typedef struct proc_node ProcessNode;
typedef struct proc_queue ProcessQueue;


ProcessNode *createProcesses(int maxProcs);

ProcessQueue *createQueue();

void deleteQueue(ProcessQueue *pq);

void enqueue(ProcessQueue *pq, ProcessNode *node);

ProcessNode *dequeue(ProcessQueue *pq);

ProcessNode *insertList(ProcessNode *p_list, ProcessNode *node);

ProcessNode *getList(ProcessNode **p_list);

void deleteList(ProcessNode *p_list);

void printQueue(ProcessQueue *pq);  //helper function

void printList(ProcessNode *p_list);  //helper function

void printProcess(ProcessNode *node);

void round_robin(ProcessNode *p_list);

#endif /* RR_H */
