/*
 * Author: Maggie Cai
 * File: utilities.h
 * Description: This header file contains the declarations of functions for the FIFO paging algorithm.
 */
#ifndef UTILITIES_H
#define UTILITIES_H

typedef struct process Process;
typedef struct proc_node ProcessNode;
typedef struct proc_queue ProcessQueue;
typedef struct page Page;
typedef struct page_list PageList;


ProcessNode *createProcesses();

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

void FIFO(ProcessNode *p_list);

#endif /* UTILITIES_H */
