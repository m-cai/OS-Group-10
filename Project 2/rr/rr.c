/*
 * Author: Maggie Cai
 * File: rr.c
 * Description: This file contains the implementation of the round robin scheduling algorithm.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rr.h"

struct process {
  int procID;
  int arrTime;
  int runTime;
  int priority;
};

struct proc_node {
  struct process *proc;
  int servTime;
  int flag;
  struct proc_node *next;
};

struct proc_queue {
  struct proc_node *head;
  struct proc_node *tail;
};

ProcessNode *createProcesses(int maxProcs)
{
  ProcessNode *p_list = malloc(sizeof(ProcessNode *));
  p_list = NULL;

  int seed = time(NULL);
  srand(seed);
  for(int i = 0; i < maxProcs; i++) {
    int id = i;
    int arrival_time = rand() % 100;
    int run_time = rand() % 11;
    if(run_time == 0) {
      run_time += 1;
    }
    int priority = rand() % 5;
    if(priority == 0) {
      priority += 1;
    }

    Process *proc = malloc(sizeof(Process *));
    proc->procID = id;
    proc->arrTime = arrival_time;
    proc->runTime = run_time;
    proc->priority = priority;

    ProcessNode *node = malloc(sizeof(ProcessNode *));
    node->proc = proc;
    node->servTime = 0;
    node->flag = 0;
    node->next = NULL;

    p_list = insertList(p_list, node);
    printProcess(node);
  }

  return p_list;
}

ProcessQueue *createQueue()
{
  ProcessQueue *pq = malloc(sizeof(ProcessQueue *)); //error
  if(pq != NULL) {
    pq->head = NULL;
    pq->tail = NULL;
  }
  return pq;
}

void deleteQueue(ProcessQueue *pq)
{
  ProcessNode *temp = NULL;
  if(pq != NULL) {
    while(pq->head != NULL)
    {
      temp = pq->head;
      pq->head = pq->head->next;
      free(temp->proc);
      free(temp);
    }
  }

  free(pq);
}

void enqueue(ProcessQueue *pq, ProcessNode *node)
{
  if(pq->head == NULL) {
    pq->head = node;
    pq->tail = node;
    node->next = NULL;
  } else {
    pq->tail->next = node;
    pq->tail = node;
    node->next = NULL;
  }
}

ProcessNode *dequeue(ProcessQueue *pq)
{
  if(pq->head == NULL) {
    return NULL;
  }

  ProcessNode *node = pq->head;
  if(pq->head == pq->tail) {
    pq->head = NULL;
    pq->tail = NULL;
  } else {
    pq->head = pq->head->next;
  }
  node->next = NULL;

  return node;
}

ProcessNode *insertList(ProcessNode *p_list, ProcessNode *node)
{
  ProcessNode *prev = NULL;
  ProcessNode *iter = p_list;

  if(p_list == NULL) {
    p_list = node;
    return p_list;
  } else {
    while(iter != NULL) {
      if(iter->proc->arrTime <= node->proc->arrTime) {
        prev = iter;
        iter = iter->next;
      } else {
        if(prev == NULL) {  //inserting at head
          node->next = p_list;
          p_list = node;
        } else {  //inserting in the middle
          node->next = iter;
          prev->next = node;
        }
        return p_list;
      }
    }
    prev->next = node;  //inserting at the end
    return p_list;
  }
}

ProcessNode *getList(ProcessNode **p_list)
{
  ProcessNode *temp = NULL;

  if(*p_list == NULL) {
    return NULL;
  }

  temp = (*p_list);
  *p_list = (*p_list)->next;
  return temp;
}

void deleteList(ProcessNode *p_list)
{
  ProcessNode *temp = NULL;
  while(p_list != NULL) {
    temp = p_list;
    p_list = p_list->next;
    free(temp->proc);
    free(temp);
  }
}

void printQueue(ProcessQueue *pq)
{
  if(pq == NULL || pq->head == NULL) {
    printf ("NULL Queue\n");
  } else {
    ProcessNode *iter = pq->head;
    while(iter != pq->tail){
      printf("P%d, ", iter->proc->procID);
      iter = iter->next;
    }
    printf("P%d", iter->proc->procID);
    printf("\n");
  }
}

void printList(ProcessNode *p_list)
{
  ProcessNode *iter = p_list;
  if(iter == NULL) {
    printf("NULL List\n");
  } else {
    while(iter != NULL) {
      printf("P%d, ", iter->proc->procID);
      iter = iter->next;
    }
    printf("\n");
  }
}

void printProcess(ProcessNode *node)
{
  if(node == NULL) {
    printf("NULL process\n");
  } else {
    printf("Process: P%d \t", node->proc->procID);
    printf("Arrival Time: %d \t", node->proc->arrTime);
    printf("Run Time: %d \t", node->proc->runTime);
    printf("Priority: %d\n", node->proc->priority);
  }
}

void round_robin(ProcessNode *p_list)
{
  int totResponseTime = 0;
  int totTAT = 0;
  int totWaitTime = 0;
  int count = 0;

  ProcessQueue *proc_queue = createQueue();
  ProcessNode *run_proc = NULL;

  // For processes with arrival time between 0 to 99
  for(int i = 0; i < 100; i++) {
    //Retrieve processes from sorted process list with arrival time = i
    while(p_list != NULL && p_list->proc->arrTime == i) {
      ProcessNode *temp = getList(&p_list);
      enqueue(proc_queue, temp);
    }

    //Check if there is currently running process
    if(run_proc != NULL) {
      if(run_proc->proc->runTime > run_proc->servTime) {
        //If the process is not finished running, add back into queue
        enqueue(proc_queue, run_proc);
      } else {
        totTAT += i - run_proc->proc->arrTime;  //TAT = Completion time - Arrival time
        totWaitTime += (i - run_proc->proc->arrTime) - run_proc->servTime;  //Wait time = TAT - Service time
        free(run_proc->proc);
        free(run_proc);
      }
    }

    //Remove the first process in the queue to run
    run_proc = dequeue(proc_queue);
    if(run_proc != NULL) {
      printf("Quantum: %d\tProcess: P%d\n", i, run_proc->proc->procID);
      run_proc->servTime++;
      if(run_proc->flag == 0) {
        totResponseTime += i - run_proc->proc->arrTime; //Response Time = First run time - Arrival time
        count += 1;
        run_proc->flag = 1;
      }
    } else {
      printf("Quantum: %d\n", i); //Idle quantum
    }
  }

  // If there are still running processes in the queue that haven't finished running yet
  int quanta = 100;
  while(run_proc != NULL) {
    if(run_proc->proc->runTime > run_proc->servTime) {
      if(run_proc->flag == 1) {
        enqueue(proc_queue, run_proc);
      } else {
        //If the process is in the queue and is the first time running, just remove it
        free(run_proc->proc);
        free(run_proc);
      }
    } else {
      totTAT += quanta  - run_proc->proc->arrTime;
      totWaitTime += (quanta - run_proc->proc->arrTime) - run_proc->servTime;
      free(run_proc->proc);
      free(run_proc);
    }

    run_proc = dequeue(proc_queue);
    if(run_proc != NULL) {
      if(run_proc->flag == 1) { //If the process has been run before
        printf("Quantum: %d\tProcess: P%d\n", quanta, run_proc->proc->procID);
        run_proc->servTime++;
        quanta += 1;
      }
    }
  }

  deleteQueue(proc_queue);
  deleteList(p_list);

  printf("----------Printing Statistics----------\n");

  float avgResponseTime = (float) totResponseTime / count;
  float avgTAT = (float) totTAT / count;
  float avgWaitTime = (float) totWaitTime / count;
  float throughput = (float) count / quanta;
  printf("Avg Response Time: %0.2f\tAvg TAT: %0.2f\tAvg Wait Time: %0.2f\n", avgResponseTime, avgTAT, avgWaitTime);
  printf("Number of Processes: %d\tThroughput: %0.2f\n", count, throughput);
}
